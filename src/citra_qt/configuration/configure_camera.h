// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "core/frontend/camera/factory.h"
#include "core/frontend/camera/interface.h"

namespace Ui {
class ConfigureCamera;
}

class ConfigureCamera : public QWidget {
    Q_OBJECT

public:
    explicit ConfigureCamera(QWidget* parent = nullptr);
    ~ConfigureCamera();

    void applyConfiguration();
    void retranslateUi();

    void timerEvent(QTimerEvent*) override;

public slots:
    /// recordConfig() and updateUiDisplay()
    void updateUI();
    void onToolButtonClicked();

private:
    enum class CameraPosition { RearRight, Front, RearLeft, RearBoth, Null };
    enum class ImageSource {
        Blank,
        StillImage,
#ifdef ENABLE_OPENCV_CAMERA
        Video, // Video & Image Sequence
#endif
        SystemCamera,
    };
    ;
    static const std::map<ConfigureCamera::ImageSource, QString> ImageSourceNames;
    static const std::map<ConfigureCamera::ImageSource, std::vector<QString>>
        ImageSourceImplementations;
    /// Record the current configuration (on a certain camera)
    void recordConfig();
    /// Updates camera mode
    void updateCameraMode();
    /// Updates image source
    void updateImageSourceUI();
    /// Load and display implementation stuff
    void loadImplementation();
    /// Update the UI according to the recorded config
    /// In most cases, recordConfig() should be called before this call, or the current config will
    /// be lost.
    void updateUiDisplay();
    void startPreviewing();
    void stopPreviewing();
    void connectEvents();
    CameraPosition getCameraSelection();
    int getSelectedCameraIndex();
    ImageSource implementationToImageSource(std::string implementation);

private:
    std::unique_ptr<Ui::ConfigureCamera> ui;
    std::array<std::string, 3> camera_name;
    std::array<std::string, 3> camera_config;
    int timer_id = 0;
    int preview_width = 0;
    int preview_height = 0;
    CameraPosition current_selected = CameraPosition::Null;
    bool is_previewing = false;
    std::unique_ptr<Camera::CameraInterface> previewing_camera;
};
