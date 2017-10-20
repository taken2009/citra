// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "citra_qt/configuration/configure_graphics.h"
#include "core/core.h"
#include "core/settings.h"
#include "ui_configure_graphics.h"

#include <QColorDialog>

ConfigureGraphics::ConfigureGraphics(QWidget* parent)
    : QWidget(parent), ui(new Ui::ConfigureGraphics) {

    ui->setupUi(this);
    this->setConfiguration();

    ui->toggle_vsync->setEnabled(!Core::System::GetInstance().IsPoweredOn());
    ui->frame_limit->setEnabled(Settings::values.use_frame_limit);
    connect(ui->toggle_frame_limit, &QCheckBox::stateChanged, ui->frame_limit,
            &QSpinBox::setEnabled);

    if (Settings::values.custom_layout || Settings::values.toggle_3d) {
        ui->layoutBox->setEnabled(false);
        if (ui->layout_combobox->currentIndex() < 4) {
            Settings::values.layout_option = Settings::LayoutOption::Stereoscopic;
        }
    } else {
        ui->layoutBox->setEnabled(true);
        if (ui->layout_combobox->currentIndex() > 3) {
            Settings::values.layout_option = Settings::LayoutOption::Default;
        }
    }

    ui->hw_renderer_group->setEnabled(ui->toggle_hw_renderer->isChecked());
    connect(ui->toggle_hw_renderer, &QCheckBox::stateChanged, ui->hw_renderer_group,
            &QWidget::setEnabled);
    ui->hw_shader_group->setEnabled(ui->toggle_hw_shader->isChecked());
    connect(ui->toggle_hw_shader, &QCheckBox::stateChanged, ui->hw_shader_group,
            &QWidget::setEnabled);

    connect(ui->layout_bg, SIGNAL (released()), this, SLOT (showLayoutBackgroundDialog()));
}

ConfigureGraphics::~ConfigureGraphics() {}

void ConfigureGraphics::showLayoutBackgroundDialog() {
    QColor new_color = QColorDialog::getColor(bg_color, this);
    if (new_color.isValid()) {
        bg_color = new_color;
        ui->layout_bg->setStyleSheet("QPushButton { background-color: " + bg_color.name() + ";}");
    }
}

void ConfigureGraphics::setConfiguration() {
    ui->toggle_hw_renderer->setChecked(Settings::values.use_hw_renderer);
    ui->toggle_hw_shader->setChecked(Settings::values.use_hw_shader);
    ui->toggle_accurate_gs->setChecked(Settings::values.shaders_accurate_gs);
    ui->toggle_accurate_mul->setChecked(Settings::values.shaders_accurate_mul);
    ui->toggle_shader_jit->setChecked(Settings::values.use_shader_jit);
    ui->resolution_factor_combobox->setCurrentIndex(Settings::values.resolution_factor);
    ui->toggle_vsync->setChecked(Settings::values.use_vsync);
    ui->toggle_frame_limit->setChecked(Settings::values.use_frame_limit);
    ui->frame_limit->setValue(Settings::values.frame_limit);
    ui->factor_3d->setValue(Settings::values.factor_3d);
    ui->toggle_3d->setChecked(Settings::values.toggle_3d);
    {
        bg_color.setRgbF(Settings::values.bg_red, Settings::values.bg_green, Settings::values.bg_blue);
        ui->layout_bg->setStyleSheet("QPushButton { background-color: " + bg_color.name() + ";}");
    }
    ui->layout_combobox->setCurrentIndex(static_cast<int>(Settings::values.layout_option));
    ui->swap_screen->setChecked(Settings::values.swap_screen);
}

void ConfigureGraphics::applyConfiguration() {
    Settings::values.use_hw_renderer = ui->toggle_hw_renderer->isChecked();
    Settings::values.use_hw_shader = ui->toggle_hw_shader->isChecked();
    Settings::values.shaders_accurate_gs = ui->toggle_accurate_gs->isChecked();
    Settings::values.shaders_accurate_mul = ui->toggle_accurate_mul->isChecked();
    Settings::values.use_shader_jit = ui->toggle_shader_jit->isChecked();
    Settings::values.resolution_factor =
        static_cast<u16>(ui->resolution_factor_combobox->currentIndex());
    Settings::values.use_vsync = ui->toggle_vsync->isChecked();
    Settings::values.use_frame_limit = ui->toggle_frame_limit->isChecked();
    Settings::values.frame_limit = ui->frame_limit->value();
    Settings::values.factor_3d = ui->factor_3d->value();
    Settings::values.toggle_3d = ui->toggle_3d->isChecked();
    if (!Settings::values.toggle_3d) {
        ui->layoutBox->setEnabled(true);
        if (ui->layout_combobox->currentIndex() > 3) {
            Settings::values.layout_option = Settings::LayoutOption::Default;
            ui->layout_combobox->setCurrentIndex(0);
        } else {
            Settings::values.layout_option =
                static_cast<Settings::LayoutOption>(ui->layout_combobox->currentIndex());
        }
    } else {
        ui->layoutBox->setEnabled(false);
        if (ui->layout_combobox->currentIndex() < 4) {
            Settings::values.layout_option = Settings::LayoutOption::Stereoscopic;
            ui->layout_combobox->setCurrentIndex(4);
        } else {
            Settings::values.layout_option =
                static_cast<Settings::LayoutOption>(ui->layout_combobox->currentIndex());
        }
    }
    Settings::values.bg_red = bg_color.redF();
    Settings::values.bg_green = bg_color.greenF();
    Settings::values.bg_blue = bg_color.blueF();
    Settings::values.layout_option =
        static_cast<Settings::LayoutOption>(ui->layout_combobox->currentIndex());
    Settings::values.swap_screen = ui->swap_screen->isChecked();
    Settings::Apply();
}

void ConfigureGraphics::retranslateUi() {
    ui->retranslateUi(this);
}
