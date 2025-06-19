#!/bin/bash

# A robust script to install Docker and Docker Compose on Debian-based systems (like Raspberry Pi OS).
# This script is based on the official Docker installation documentation.
# It will exit immediately if any command fails.
set -e

echo "Starting Docker Installation Script..."

# --- STEP 1: Update packages and install prerequisites ---
echo "Step 1: Updating packages and installing prerequisites..."
sudo apt-get update
sudo apt-get install -y ca-certificates curl

# --- STEP 2: Add Docker's official GPG key ---
# This step ensures that the Docker packages you download are authentic.
echo "Step 2: Adding Docker's official GPG key..."
# Create the directory for keyring if it doesn't exist
sudo install -m 0755 -d /etc/apt/keyrings
# Download the Docker GPG key
sudo curl -fsSL https://download.docker.com/linux/raspbian/gpg -o /etc/apt/keyrings/docker.asc
# Ensure the key is readable by the apt package manager
sudo chmod a+r /etc/apt/keyrings/docker.asc

# --- STEP 3: Add the Docker repository to Apt sources ---
echo "Step 3: Adding the Docker repository to Apt sources..."
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/raspbian \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# --- STEP 4: Install Docker Engine and Plugins ---
echo "Step 4: Updating package list and installing Docker Engine..."
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

echo "Step 5: Performing post-installation steps..."

# Add the current user to the 'docker' group to run docker commands without sudo.
# This is a crucial quality-of-life improvement.
if ! getent group docker > /dev/null; then
    sudo groupadd docker
    echo "Created 'docker' group."
fi

sudo usermod -aG docker $USER
echo "Added user '$USER' to the 'docker' group."

echo ""
echo "Docker Installation Complete!"
echo "--------------------------------------------------"
echo "IMPORTANT:"
echo "For the group changes to take effect, you need to either:"
echo "1. Log out and log back in."
echo "2. Or run the following command in your current terminal:"
echo "   newgrp docker"
echo ""
echo "After doing so, you can verify the installation by running:"
echo "   docker --version"
echo "   docker compose version"
echo "--------------------------------------------------"