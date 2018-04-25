// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>
#include <QDirIterator>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QWidget>
#include "citra_qt/configuration/configure_camera.h"
#include "citra_qt/ui_settings.h"
#include "core/core.h"
#include "core/settings.h"
#include "ui_configure_camera.h"

#ifdef ENABLE_OPENCV_CAMERA
const std::map<ConfigureCamera::ImageSource, QString> ConfigureCamera::ImageSourceNames = {
    {ImageSource::Blank, QObject::tr("Blank")},
    {ImageSource::StillImage, QObject::tr("Still Image")},
    {ImageSource::Video, QObject::tr("Video & Image Sequence")},
    {ImageSource::SystemCamera, QObject::tr("System Camera")},
};
const std::map<ConfigureCamera::ImageSource, std::vector<QString>>
    ConfigureCamera::ImageSourceImplementations = {
        {ImageSource::Blank, {"blank"}},
        {ImageSource::StillImage, {"image"}},
        {ImageSource::Video, {"opencv"}},
        {ImageSource::SystemCamera, {"opencv", "qt"}},
};
#else
const std::map<ConfigureCamera::ImageSource, QString> ConfigureCamera::ImageSourceNames = {
    {ImageSource::Blank, QObject::tr("Blank")},
    {ImageSource::StillImage, QObject::tr("Still Image")},
    {ImageSource::SystemCamera, QObject::tr("System Camera")},
};
const std::map<ConfigureCamera::ImageSource, std::vector<QString>>
    ConfigureCamera::ImageSourceImplementations = {
        {ImageSource::Blank, {"blank"}},
        {ImageSource::StillImage, {"image"}},
        {ImageSource::SystemCamera, {"qt"}},
};
#endif

ConfigureCamera::ConfigureCamera(QWidget* parent)
    : QWidget(parent), ui(std::make_unique<Ui::ConfigureCamera>()) {
    ui->setupUi(this);
    // Load settings
    camera_name = Settings::values.camera_name;
    camera_config = Settings::values.camera_config;
    for (auto pair : ImageSourceImplementations) {
        if (!pair.second.empty()) {
            ui->image_source->addItem(ImageSourceNames.at(pair.first));
        }
    }
    updateCameraMode();
    updateUiDisplay();
    loadImplementation();
    connectEvents();
}

ConfigureCamera::~ConfigureCamera() {
    stopPreviewing();
}

void ConfigureCamera::connectEvents() {
    connect(ui->image_source,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this] {
                stopPreviewing();
                updateImageSourceUI();
                loadImplementation();
            });
    connect(ui->camera_selection,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this] {
                updateCameraMode();
                updateUI();
                loadImplementation();
            });
    connect(ui->camera_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this] {
                updateUI();
                loadImplementation();
            });
    connect(ui->camera_position,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this] {
                updateUI();
                loadImplementation();
            });
    connect(ui->toolButton, &QToolButton::clicked, this, &ConfigureCamera::onToolButtonClicked);
    connect(ui->preview_button, &QPushButton::clicked, this, [=] { startPreviewing(); });
    connect(ui->prompt_before_load, &QCheckBox::stateChanged, this, [this](int state) {
        ui->camera_file->setDisabled(state == Qt::Checked);
        ui->toolButton->setDisabled(state == Qt::Checked);
        if (state == Qt::Checked) {
            ui->camera_file->setText("");
        }
    });
    connect(ui->implementation,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [this] { stopPreviewing(); });
}

void ConfigureCamera::updateCameraMode() {
    if (getCameraSelection() != CameraPosition::Front) {
        ui->camera_mode->setCurrentIndex(1); // Double
        if (camera_name[0] == camera_name[2] && camera_config[0] == camera_config[2]) {
            ui->camera_mode->setCurrentIndex(0); // Single
        }
    }
}

void ConfigureCamera::updateUiDisplay() {
    stopPreviewing();
    CameraPosition pos = getCameraSelection();
    current_selected = pos;
    int camera_selection = getSelectedCameraIndex();
    // Set the visibility of the camera mode selection widgets
    if (pos == CameraPosition::RearBoth) {
        ui->camera_position->setHidden(true);
        ui->camera_position_label->setHidden(true);
        ui->camera_mode->setHidden(false);
        ui->camera_mode_label->setHidden(false);
    } else {
        ui->camera_position->setHidden(pos == CameraPosition::Front);
        ui->camera_position_label->setHidden(pos == CameraPosition::Front);
        ui->camera_mode->setHidden(pos == CameraPosition::Front);
        ui->camera_mode_label->setHidden(pos == CameraPosition::Front);
    }
    ImageSource image_source = implementationToImageSource(camera_name[camera_selection]);
    // Set camera config text
    ui->camera_file->setText(QString::fromStdString(camera_config[camera_selection]));
    ui->image_source->setCurrentText(ImageSourceNames.at(image_source));
    updateImageSourceUI();
}

void ConfigureCamera::updateImageSourceUI() {
    auto image_source = static_cast<ImageSource>(ui->image_source->currentIndex());
    switch (image_source) {
    case ImageSource::Blank:
    case ImageSource::SystemCamera:
        ui->prompt_before_load->setHidden(true);
        ui->prompt_before_load->setChecked(false);
        ui->camera_file_label->setHidden(true);
        ui->camera_file->setHidden(true);
        ui->camera_file->setText("");
        ui->toolButton->setHidden(true);
        break;
    case ImageSource::StillImage:
        ui->prompt_before_load->setHidden(false);
        ui->camera_file_label->setHidden(false);
        ui->camera_file->setHidden(false);
        ui->toolButton->setHidden(false);
        if (camera_config[getSelectedCameraIndex()].empty()) {
            ui->prompt_before_load->setChecked(true);
            ui->camera_file->setDisabled(true);
            ui->toolButton->setDisabled(true);
            ui->camera_file->setText("");
        } else {
            ui->camera_file->setDisabled(false);
            ui->toolButton->setDisabled(false);
        }
        break;
#ifdef ENABLE_OPENCV_CAMERA
    case ImageSource::Video:
        ui->prompt_before_load->setHidden(true);
        ui->prompt_before_load->setChecked(false);
        ui->camera_file_label->setHidden(false);
        ui->camera_file->setHidden(false);
        ui->toolButton->setHidden(false);
        break;
#endif
    }
}

void ConfigureCamera::loadImplementation() {
    int camera_selection = getSelectedCameraIndex();
    ui->implementation->clear();
    for (const auto& implementation : ImageSourceImplementations.at(
             static_cast<ImageSource>(ui->image_source->currentIndex()))) {
        ui->implementation->addItem(implementation);
        if (camera_name[camera_selection] == implementation.toStdString()) {
            ui->implementation->setCurrentText(
                QString::fromStdString(camera_name[camera_selection]));
        }
    }
    if (ui->implementation->currentIndex() == -1) {
        ui->implementation->setCurrentIndex(0);
    }
}

void ConfigureCamera::recordConfig() {
    if (current_selected == CameraPosition::RearBoth) {
        camera_name[0] = camera_name[2] = ui->implementation->currentText().toStdString();
        camera_config[0] = camera_config[2] = ui->camera_file->text().toStdString();
    } else if (current_selected != CameraPosition::Null) {
        int index = static_cast<int>(current_selected);
        camera_name[index] = ui->implementation->currentText().toStdString();
        camera_config[index] = ui->camera_file->text().toStdString();
    }
}

void ConfigureCamera::startPreviewing() {
    recordConfig();
    int camera_selection = getSelectedCameraIndex();
    stopPreviewing();
    // Init preview box
    ui->preview_box->setHidden(false);
    ui->preview_button->setHidden(true);
    preview_width = ui->preview_box->size().width();
    preview_height = preview_width * 0.75;
    ui->preview_box->setToolTip(tr("Resolution: ") + QString::number(preview_width) + "*" +
                                QString::number(preview_height));
    // Load previewing camera
    previewing_camera =
        Camera::CreateCameraPreview(camera_name[camera_selection], camera_config[camera_selection],
                                    preview_width, preview_height);
    if (!previewing_camera) {
        return;
    }
    previewing_camera->SetResolution(
        {static_cast<u16>(preview_width), static_cast<u16>(preview_height)});
    previewing_camera->SetEffect(Service::CAM::Effect::None);
    previewing_camera->SetFlip(Service::CAM::Flip::None);
    previewing_camera->SetFormat(Service::CAM::OutputFormat::RGB565);
    previewing_camera->SetFrameRate(Service::CAM::FrameRate::Rate_30);
    previewing_camera->StartCapture();

    timer_id = startTimer(1000 / 30);
}

void ConfigureCamera::stopPreviewing() {
    ui->preview_box->setHidden(true);
    ui->preview_button->setHidden(false);

    if (previewing_camera) {
        previewing_camera->StopCapture();
    }

    if (timer_id != 0) {
        killTimer(timer_id);
        timer_id = 0;
    }
}

void ConfigureCamera::timerEvent(QTimerEvent* event) {
    if (event->timerId() != timer_id) {
        return;
    }
    if (!previewing_camera) {
        killTimer(timer_id);
        timer_id = 0;
        return;
    }
    std::vector<u16> frame = previewing_camera->ReceiveFrame();
    int width = ui->preview_box->size().width();
    int height = width * 0.75;
    if (width != preview_width || height != preview_height) {
        stopPreviewing();
        return;
    }
    QImage image(width, height, QImage::Format::Format_RGB16);
    std::memcpy(image.bits(), frame.data(), width * height * sizeof(u16));
    ui->preview_box->setPixmap(QPixmap::fromImage(image));
}

void ConfigureCamera::updateUI() {
    recordConfig();
    updateUiDisplay();
}

void ConfigureCamera::onToolButtonClicked() {
    int camera_selection = getSelectedCameraIndex();
    QString filter;
    if (camera_name[camera_selection] == "image") {
        QList<QByteArray> types = QImageReader::supportedImageFormats();
        QList<QString> temp_filters;
        for (const QByteArray& type : types) {
            temp_filters << QString("*." + QString(type));
        }
        filter = tr("Supported image files (%1)").arg(temp_filters.join(" "));
    }
#ifdef ENABLE_OPENCV_CAMERA
    else if (camera_name[camera_selection] == "opencv") {
        filter = tr("Supported video files (*.mpg *.avi)");
    }
#endif
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", filter);
    if (!path.isEmpty()) {
        ui->camera_file->setText(path);
    }
}

void ConfigureCamera::applyConfiguration() {
    recordConfig();
    stopPreviewing();
    Settings::values.camera_name = camera_name;
    Settings::values.camera_config = camera_config;
    Settings::Apply();
}

ConfigureCamera::CameraPosition ConfigureCamera::getCameraSelection() {
    switch (ui->camera_selection->currentIndex()) {
    case 0: // Front
        return CameraPosition::Front;
    case 1: // Rear
        if (ui->camera_mode->currentIndex() == 0) {
            // Single (2D) mode
            return CameraPosition::RearBoth;
        } else {
            // Double (3D) mode
            return (ui->camera_position->currentIndex() == 0) ? CameraPosition::RearLeft
                                                              : CameraPosition::RearRight;
        }
    default:
        NGLOG_ERROR(Frontend, "Unknown camera selection");
        return CameraPosition::Front;
    }
}

int ConfigureCamera::getSelectedCameraIndex() {
    CameraPosition pos = getCameraSelection();
    int camera_selection = static_cast<int>(pos);
    if (pos == CameraPosition::RearBoth) { // Single Mode
        camera_selection = 0;              // Either camera is the same, so we return RearRight
    }
    return camera_selection;
}

void ConfigureCamera::retranslateUi() {
    ui->retranslateUi(this);
}

ConfigureCamera::ImageSource ConfigureCamera::implementationToImageSource(
    std::string implementation) {
    int index = getSelectedCameraIndex();
    // Convert camera name to image sources
    if (implementation == "blank") {
        return ImageSource::Blank;
    } else if (implementation == "image") {
        return ImageSource::StillImage;
    }
#ifdef ENABLE_OPENCV_CAMERA
    else if (implementation == "opencv") {
        if (camera_config[index].empty()) {
            return ImageSource::SystemCamera;
        } else {
            return ImageSource::Video;
        }
    }
#endif
    else if (implementation == "qt") {
        return ImageSource::SystemCamera;
    } else {
        NGLOG_ERROR(Frontend, "Unknown camera type {}", implementation.c_str());
        QString message = tr(("Sorry, but your configuration file seems to be invalid:\n\nUnknown "
                              "camera implementation " +
                              implementation)
                                 .c_str());
#ifndef ENABLE_OPENCV_CAMERA
        if (implementation == "opencv") {
            message.append(tr("\n\nThis build does not have the OpenCV camera included."));
#if defined(Q_OS_WIN64) && defined(Q_CC_MSVC)
            message.append(tr("\nPlease look for a mingw build instead if you really want to use "
                              "it, but it is more recommended that you use the new QtMultimedia "
                              "based camera implementations."));
#elif !defined(Q_OS_UNIX) && !defined(Q_OS_WIN64)
            message.append(tr("\nSorry, but currently Citra does not have an OpenCV-integrated "
                              "build for your platform."));
            message.append(
                tr("\nYou may switch to our new QtMultimedia based camera implementations."));
#endif
        }
#endif
        QMessageBox::critical(this, tr("Error"), message);
        return ImageSource::Blank;
    }
}
