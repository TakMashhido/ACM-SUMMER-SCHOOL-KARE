import pandas as pd
import numpy as np
import joblib
import argparse
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error, mean_absolute_error

import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader

# --- Configuration & Hyperparameters for a Small Dataset ---
# Use the last 60 data points (2 minutes) to predict the next one (2 seconds ahead).
LOOK_BACK = 60
EPOCHS = 100  # We can use fewer epochs for smaller data
BATCH_SIZE = 16 # A smaller batch size is often better for smaller datasets
PATIENCE = 10   # For Early Stopping
LEARNING_RATE = 0.001

# --- Setup Device (CPU or GPU) ---
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"Using device: {device}")


def load_and_preprocess_data(filepath):
    """
    Loads short-term data, splits, and scales it.
    """
    try:
        data = pd.read_csv(filepath)
    except FileNotFoundError:
        print(f"Error: The file '{filepath}' was not found.")
        return None
    
    if len(data) < LOOK_BACK * 4: # Check if there's enough data for a meaningful split
        print(f"Error: Not enough data to train. Need at least {LOOK_BACK * 4} rows, but found {len(data)}.")
        return None

    # We assume the timestamp column exists, but we don't need it for feature engineering here.
    # The features are just temperature and humidity.
    features = data[['temperature', 'humidity']]
    
    # --- Train, Validation, Test Split ---
    n = len(features)
    train_df = features[0:int(n*0.7)]
    val_df = features[int(n*0.7):int(n*0.85)]
    test_df = features[int(n*0.85):]
    
    # --- Scaling ---
    feature_scaler = MinMaxScaler(feature_range=(0, 1))
    target_scaler = MinMaxScaler(feature_range=(0, 1))

    # Fit scalers only on training data
    scaled_train = feature_scaler.fit_transform(train_df)
    target_scaler.fit(train_df[['temperature']])

    # Transform all sets
    scaled_val = feature_scaler.transform(val_df)
    scaled_test = feature_scaler.transform(test_df)
    
    return scaled_train, scaled_val, scaled_test, feature_scaler, target_scaler


# --- PyTorch Dataset Definition (No changes needed) ---
class WeatherDataset(Dataset):
    def __init__(self, data, look_back):
        self.data = data
        self.look_back = look_back
    
    def __len__(self):
        return len(self.data) - self.look_back - 1
    
    def __getitem__(self, idx):
        X = self.data[idx:(idx + self.look_back), :]
        y = self.data[idx + self.look_back, 0] # Target is temperature (first column)
        return torch.tensor(X, dtype=torch.float32), torch.tensor(y, dtype=torch.float32)


# --- PyTorch Model Definition (Slightly simplified) ---
class LSTMModel(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers, output_size, dropout_prob):
        super(LSTMModel, self).__init__()
        # A single LSTM layer might be sufficient for this amount of data
        self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True)
        self.dropout = nn.Dropout(dropout_prob)
        self.fc = nn.Linear(hidden_size, output_size)
    
    def forward(self, x):
        out, _ = self.lstm(x)
        out = self.dropout(out[:, -1, :]) # Get output from the last time step
        out = self.fc(out)
        return out


# --- Plotting Functions (Updated labels) ---
def plot_history(train_loss, val_loss):
    plt.figure(figsize=(10, 6))
    plt.plot(train_loss, label='Training Loss')
    plt.plot(val_loss, label='Validation Loss')
    plt.title('Model Loss Over Epochs')
    plt.ylabel('Loss (MSE)')
    plt.xlabel('Epoch')
    plt.legend()
    plt.savefig('training_history_short_term.png')
    print("Saved training history plot to training_history_short_term.png")

def plot_predictions(y_true, y_pred):
    plt.figure(figsize=(15, 7))
    plt.plot(y_true, color='blue', label='Actual Temperature')
    plt.plot(y_pred, color='red', linestyle='--', label='Predicted Temperature')
    plt.title('Short-Term Temperature Forecast vs Actuals')
    plt.ylabel('Temperature (°C)')
    plt.xlabel('Time Steps (2-second intervals)')
    plt.legend()
    plt.savefig('test_predictions_short_term.png')
    print("Saved test prediction plot to test_predictions_short_term.png")


# --- Main Execution Block ---
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Short-Term LSTM Weather Forecasting Model Trainer (PyTorch).')
    parser.add_argument('--file', type=str, default='D:\GitHub\ACM-SUMMER-SCHOOL-KARE\Projects\Fog Computing-Based Weather Forecasting with NodeMCU, Raspberry Pi, and LSTM\Data Collection\weather_data.csv', help='Path to the weather data CSV file.')
    args = parser.parse_args()

    # 1. Load and process data
    processed_data = load_and_preprocess_data(args.file)
    if processed_data is None:
        exit()
    scaled_train, scaled_val, scaled_test, feature_scaler, target_scaler = processed_data
    
    # 2. Create PyTorch Datasets and DataLoaders
    train_dataset = WeatherDataset(scaled_train, LOOK_BACK)
    val_dataset = WeatherDataset(scaled_val, LOOK_BACK)
    test_dataset = WeatherDataset(scaled_test, LOOK_BACK)
    
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False)
    test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False)

    # 3. Build the model
    num_features = scaled_train.shape[1] # Should be 2 (temp and humidity)
    model = LSTMModel(input_size=num_features, hidden_size=50, num_layers=1, output_size=1, dropout_prob=0.2).to(device)
    print(model)

    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LEARNING_RATE)

    # 4. Training Loop
    best_val_loss = float('inf')
    epochs_no_improve = 0
    train_losses, val_losses = [], []

    for epoch in range(EPOCHS):
        model.train()
        running_train_loss = 0.0
        for inputs, targets in train_loader:
            inputs, targets = inputs.to(device), targets.to(device)
            outputs = model(inputs)
            loss = criterion(outputs.squeeze(), targets)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            running_train_loss += loss.item()
        
        avg_train_loss = running_train_loss / len(train_loader)
        train_losses.append(avg_train_loss)
        
        # Validation loop
        model.eval()
        running_val_loss = 0.0
        with torch.no_grad():
            for inputs, targets in val_loader:
                inputs, targets = inputs.to(device), targets.to(device)
                outputs = model(inputs)
                loss = criterion(outputs.squeeze(), targets)
                running_val_loss += loss.item()
        
        avg_val_loss = running_val_loss / len(val_loader)
        val_losses.append(avg_val_loss)
        
        print(f"Epoch [{epoch+1}/{EPOCHS}], Train Loss: {avg_train_loss:.6f}, Val Loss: {avg_val_loss:.6f}")
        
        if avg_val_loss < best_val_loss:
            best_val_loss = avg_val_loss
            torch.save(model.state_dict(), 'best_short_term_model.pth')
            epochs_no_improve = 0
            print(f"Validation loss improved. Saving model to 'best_short_term_model.pth'")
        else:
            epochs_no_improve += 1
        
        if epochs_no_improve >= PATIENCE:
            print("Early stopping triggered.")
            break
            
    # 5. Evaluate the BEST model on the test set
    print("\n--- Evaluating Best Model on Test Data ---")
    model.load_state_dict(torch.load('best_short_term_model.pth'))
    model.eval()
    
    all_targets = []
    all_predictions = []
    with torch.no_grad():
        for inputs, targets in test_loader:
            inputs, targets = inputs.to(device), targets.to(device)
            outputs = model(inputs)
            all_predictions.extend(outputs.cpu().numpy())
            all_targets.extend(targets.cpu().numpy())
            
    predictions_unscaled = target_scaler.inverse_transform(np.array(all_predictions))
    targets_unscaled = target_scaler.inverse_transform(np.array(all_targets).reshape(-1, 1))

    rmse = np.sqrt(mean_squared_error(targets_unscaled, predictions_unscaled))
    mae = mean_absolute_error(targets_unscaled, predictions_unscaled)
    print(f"Test Set Root Mean Squared Error (RMSE): {rmse:.2f} °C")
    print(f"Test Set Mean Absolute Error (MAE): {mae:.2f} °C")

    # 6. Visualize results
    plot_history(train_losses, val_losses)
    plot_predictions(targets_unscaled, predictions_unscaled)
    
    # 7. Save scalers for deployment
    joblib.dump(feature_scaler, 'feature_scaler.gz')
    joblib.dump(target_scaler, 'target_scaler.gz')
    print("\nModel training complete. Best model saved as 'best_short_term_model.pth'.")
    print("Scalers saved as 'feature_scaler.gz' and 'target_scaler.gz'.")