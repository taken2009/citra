// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QCamera>
#include <QImageReader>
#include <QMessageBox>
#include <QThread>
#include "citra_qt/camera/qt_multimedia_camera.h"
#include "citra_qt/main.h"

namespace Camera {

QList<QVideoFrame::PixelFormat> QtCameraSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
           << QVideoFrame::Format_ARGB32 << QVideoFrame::Format_ARGB32_Premultiplied
           << QVideoFrame::Format_RGB32 << QVideoFrame::Format_RGB24 << QVideoFrame::Format_RGB565
           << QVideoFrame::Format_RGB555 << QVideoFrame::Format_ARGB8565_Premultiplied
           << QVideoFrame::Format_BGRA32 << QVideoFrame::Format_BGRA32_Premultiplied
           << QVideoFrame::Format_BGR32 << QVideoFrame::Format_BGR24 << QVideoFrame::Format_BGR565
           << QVideoFrame::Format_BGR555 << QVideoFrame::Format_BGRA5658_Premultiplied
           << QVideoFrame::Format_AYUV444 << QVideoFrame::Format_AYUV444_Premultiplied
           << QVideoFrame::Format_YUV444 << QVideoFrame::Format_YUV420P << QVideoFrame::Format_YV12
           << QVideoFrame::Format_UYVY << QVideoFrame::Format_YUYV << QVideoFrame::Format_NV12
           << QVideoFrame::Format_NV21 << QVideoFrame::Format_IMC1 << QVideoFrame::Format_IMC2
           << QVideoFrame::Format_IMC3 << QVideoFrame::Format_IMC4 << QVideoFrame::Format_Y8
           << QVideoFrame::Format_Y16 << QVideoFrame::Format_Jpeg << QVideoFrame::Format_CameraRaw
           << QVideoFrame::Format_AdobeDng; // Supporting all the formats
}

bool QtCameraSurface::present(const QVideoFrame& frame) {
    if (!frame.isValid()) {
        return false;
    }
    QVideoFrame cloneFrame(frame);
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
    const QImage image(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(),
                       QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
    current_frame = image.copy();
    cloneFrame.unmap();
    return true;
}

QtMultimediaCamera::QtMultimediaCamera(const std::string& camera_name) {
    if (QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->thread() ==
        QThread::currentThread()) {
        QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->CreateCamera();
    } else {
        QMetaObject::invokeMethod(QtMultimediaCameraHandler::g_qt_multimedia_camera_handler.get(),
                                  "CreateCamera", Qt::BlockingQueuedConnection);
    }
}

QtMultimediaCamera::~QtMultimediaCamera() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StopCamera();
}

void QtMultimediaCamera::StartCapture() {
    if (QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->thread() ==
        QThread::currentThread()) {
        QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StartCamera();
    } else {
        QMetaObject::invokeMethod(QtMultimediaCameraHandler::g_qt_multimedia_camera_handler.get(),
                                  "StartCamera", Qt::BlockingQueuedConnection);
    }
}

void QtMultimediaCamera::StopCapture() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StopCamera();
}

void QtMultimediaCamera::SetFormat(Service::CAM::OutputFormat output_format) {
    output_rgb = output_format == Service::CAM::OutputFormat::RGB565;
}

void QtMultimediaCamera::SetResolution(const Service::CAM::Resolution& resolution) {
    width = resolution.width;
    height = resolution.height;
}

void QtMultimediaCamera::SetFlip(Service::CAM::Flip flip) {
    using namespace Service::CAM;
    flip_horizontal = (flip == Flip::Horizontal) || (flip == Flip::Reverse);
    flip_vertical = (flip == Flip::Vertical) || (flip == Flip::Reverse);
}

void QtMultimediaCamera::SetEffect(Service::CAM::Effect effect) {
    if (effect != Service::CAM::Effect::None) {
        LOG_ERROR(Service_CAM, "Unimplemented effect %d", static_cast<int>(effect));
    }
}

std::vector<u16> QtMultimediaCamera::ReceiveFrame() {
    return CameraUtil::ProcessImage(
        QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->camera_surface.current_frame,
        width, height, output_rgb, flip_horizontal, flip_vertical);
}

void QtMultimediaCamera::OnServicePaused() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StopCamera();
}

void QtMultimediaCamera::OnServiceResumed() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StartCamera();
}

void QtMultimediaCamera::OnServiceStopped() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->StopCamera();
}

bool QtMultimediaCamera::IsPreviewAvailable() {
    return QtMultimediaCameraHandler::g_qt_multimedia_camera_handler->CameraAvailable();
}

std::unique_ptr<CameraInterface> QtMultimediaCameraFactory::Create(
    const std::string& config) const {
    return std::make_unique<QtMultimediaCamera>(config);
}

std::unique_ptr<QtMultimediaCameraHandler>
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler;

void QtMultimediaCameraHandler::Init() {
    QtMultimediaCameraHandler::g_qt_multimedia_camera_handler =
        std::make_unique<QtMultimediaCameraHandler>();
}

void QtMultimediaCameraHandler::CreateCamera() {
    camera = std::make_unique<QCamera>();
    camera->setViewfinder(&camera_surface);
}

void QtMultimediaCameraHandler::StopCamera() {
    camera->stop();
}

void QtMultimediaCameraHandler::StartCamera() {
    camera->start();
}

bool QtMultimediaCameraHandler::CameraAvailable() const {
    return camera->isAvailable();
}

} // namespace Camera
