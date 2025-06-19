FROM python:3.9-slim-buster

# Set the working directory inside the container
WORKDIR /app

# Copy the file containing your Python dependencies
COPY requirements.txt .

# Install any needed packages specified in requirements.txt
# --no-cache-dir reduces the image size
RUN pip install --no-cache-dir -r requirements.txt

# Copy your CoAP server script into the container at /app
COPY server.py .

# Expose the standard CoAP port (UDP).
# This informs Docker that the container listens on this network port.
EXPOSE 5683/udp

# Define the command to run your application
CMD [ "python", "./server.py" ]
