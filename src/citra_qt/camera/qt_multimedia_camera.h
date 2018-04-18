// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>
#include <vector>
#include <QAbstractVideoSurface>
#include <QCamera>
#include <QImage>
#include "citra_qt/camera/camera_util.h"
#include "citra_qt/camera/qt_camera_factory.h"
#include "core/frontend/camera/interface.h"

class GMainWindow;

namespace Camera {

class QtCameraSurface final : public QAbstractVideoSurface {
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType) const override;
    bool present(const QVideoFrame&) override;

private:
    QImage current_frame;

    friend class QtMultimediaCamera; // For access to current_frame
};

/// This class is only an interface. It just calls QtMultimediaCameraHandler.
class QtMultimediaCamera final : public CameraInterface {
public:
    QtMultimediaCamera(const std::string& camera_name);
    ~QtMultimediaCamera();
    void StartCapture() override;
    void StopCapture() override;
    void SetResolution(const Service::CAM::Resolution&) override;
    void SetFlip(Service::CAM::Flip) override;
    void SetEffect(Service::CAM::Effect) override;
    void SetFormat(Service::CAM::OutputFormat) override;
    void SetFrameRate(Service::CAM::FrameRate frame_rate) override {}
    std::vector<u16> ReceiveFrame() override;
    void OnServicePaused() override;
    void OnServiceResumed() override;
    void OnServiceStopped() override;
    bool IsPreviewAvailable() override;

private:
    QCamera* camera;
    QtCameraSurface camera_surface;
    int width, height;
    bool output_rgb;
    bool flip_horizontal, flip_vertical;
};

class QtMultimediaCameraFactory final : public QtCameraFactory {
public:
    std::unique_ptr<CameraInterface> Create(const std::string& config) const override;
};

class QtMultimediaCameraHandler final : public QObject {
    Q_OBJECT

public:
    /// Creates the global handler. Must be called in UI thread.
    static void Init();

    /**
     * Creates the camera.
     * Note: This function must be called via QMetaObject::invokeMethod in UI thread.
     */
    Q_INVOKABLE void CreateCamera();

    /**
     * Starts the camera.
     * Note: This function must be called via QMetaObject::invokeMethod in UI thread when
     *       starting the camera for the first time. 'Resume' calls can be in other threads.
     */
    Q_INVOKABLE void StartCamera();

    void StopCamera();
    bool CameraAvailable() const;

    /// Pointer to the global QtMultimediaCameraHandler
    static std::unique_ptr<QtMultimediaCameraHandler> g_qt_multimedia_camera_handler;

private:
    std::unique_ptr<QCamera> camera;
    QtCameraSurface camera_surface{};

    friend class QtMultimediaCamera; // For access to camera_surface (and camera)
};

} // namespace Camera
