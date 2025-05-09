# Omi App

The Omi App is a Flutter-based mobile application that serves as the companion app for Omi devices. This app enables users to interact with their Omi device, manage apps, and customize their experience.

## 📚 **[View Full App setup instructions in the documentation](https://docs.omi.me/docs/developer/AppSetup)**

### Quick Setup

1. Navigate to the app directory:
   ```bash
   cd app
   ```

2. Run setup script:
   ```bash
   # For iOS
   bash setup.sh ios

   # For Android
   bash setup.sh android
   ```

3. Run the app:
   ```bash
   flutter run --flavor dev
   ```

### Building and Deploying to iPhone

To build and deploy the app to an iPhone so it can run independently from your laptop:

1. Build the iOS app with release mode and specific flavor:
   ```bash
   flutter build ios --flavor dev --release
   ```
   This produces an .app bundle at:
   ```
   build/ios/iphoneos/Runner.app
   ```

2. **Install directly from the .app bundle (recommended for local device install):**
   ```bash
   ios-deploy --bundle build/ios/iphoneos/Runner.app --debug
   ```
   This will install the app directly to your connected iPhone.

3. **Build and deploy in a single command:**
   ```bash
   flutter build ios --flavor dev --release && ios-deploy --bundle build/ios/iphoneos/Runner.app
   ```

   Prerequisites:
   - Ensure your iPhone is connected via USB
   - Install ios-deploy if not already installed: `brew install ios-deploy`
   - Make sure your iPhone is trusted on your Mac
   - Verify your development certificate is valid and provisioning profiles are set up

Once installed, the app will run on your iPhone independently from your development machine.

### Attaching to Running Process

To debug an already running app on your device:

1. Find the process ID of your running app:
   ```bash
   flutter devices
   ```

2. Attach the debugger to the running app:
   ```bash
   flutter attach --device-id=<DEVICE_ID>
   ```

3. Once attached, you can:
   - Use hot reload (press 'r')
   - Use hot restart (press 'R')
   - Access debug information
   - Set breakpoints if using an IDE like VS Code or Android Studio

## Need Help?

- 💬 Join our [Discord Community](http://discord.omi.me)
