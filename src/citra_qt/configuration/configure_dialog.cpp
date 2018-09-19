// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QHash>
#include <QListWidgetItem>
#include "citra_qt/configuration/config.h"
#include "citra_qt/configuration/configure_dialog.h"
#include "citra_qt/hotkeys.h"
#include "core/settings.h"
#include "ui_configure.h"

ConfigureDialog::ConfigureDialog(QWidget* parent, const HotkeyRegistry& registry)
    : QDialog(parent), ui(new Ui::ConfigureDialog) {
    ui->setupUi(this);
    ui->hotkeysTab->Populate(registry);
    this->setConfiguration();
    this->PopulateSelectionList();
    connect(ui->generalTab, &ConfigureGeneral::languageChanged, this,
            &ConfigureDialog::onLanguageChanged);

    connect(ui->selectorList, &QListWidget::itemSelectionChanged, this,
            &ConfigureDialog::UpdateVisibleTabs);
    adjustSize();
    ui->selectorList->setCurrentRow(0);

    connect(ui->inputTab, &ConfigureInput::InputKeysChanged, ui->hotkeysTab,
            &ConfigureHotkeys::OnInputKeysChanged);
    connect(ui->hotkeysTab, &ConfigureHotkeys::HotkeysChanged, ui->inputTab,
            &ConfigureInput::OnHotkeysChanged);

    // Synchronise lists upon initialisation
    ui->inputTab->EmitInputKeysChanged();
    ui->hotkeysTab->EmitHotkeysChanged();
}

ConfigureDialog::~ConfigureDialog() = default;

void ConfigureDialog::setConfiguration() {}

void ConfigureDialog::applyConfiguration(HotkeyRegistry& registry) {
    ui->generalTab->applyConfiguration();
    ui->systemTab->applyConfiguration();
    ui->inputTab->applyConfiguration();
    ui->hotkeysTab->applyConfiguration(registry);
    ui->graphicsTab->applyConfiguration();
    ui->audioTab->applyConfiguration();
    ui->cameraTab->applyConfiguration();
    ui->debugTab->applyConfiguration();
    ui->webTab->applyConfiguration();
    Settings::Apply();
    Settings::LogSettings();
}

void ConfigureDialog::PopulateSelectionList() {

    const std::array<std::pair<QString, QStringList>, 4> items{
        {{tr("General"), {tr("General"), tr("Web"), tr("Debug")}},
         {tr("System"), {tr("System"), tr("Audio"), tr("Camera")}},
         {tr("Graphics"), {tr("Graphics")}},
         {tr("Controls"), {tr("Input"), tr("Hotkeys")}}}};

    for (const auto& entry : items) {
        auto* item = new QListWidgetItem(entry.first);
        item->setData(Qt::UserRole, entry.second);

        ui->selectorList->addItem(item);
    }
}

void ConfigureDialog::onLanguageChanged(const QString& locale) {
    emit languageChanged(locale);
    ui->retranslateUi(this);
    ui->generalTab->retranslateUi();
    ui->systemTab->retranslateUi();
    ui->inputTab->retranslateUi();
    ui->hotkeysTab->retranslateUi();
    ui->graphicsTab->retranslateUi();
    ui->audioTab->retranslateUi();
    ui->cameraTab->retranslateUi();
    ui->debugTab->retranslateUi();
    ui->webTab->retranslateUi();
}

void ConfigureDialog::UpdateVisibleTabs() {
    auto items = ui->selectorList->selectedItems();
    if (items.isEmpty())
        return;

    const QHash<QString, QWidget*> widgets = {{tr("General"), ui->generalTab},
                                              {tr("System"), ui->systemTab},
                                              {tr("Input"), ui->inputTab},
                                              {tr("Hotkeys"), ui->hotkeysTab},
                                              {tr("Graphics"), ui->graphicsTab},
                                              {tr("Audio"), ui->audioTab},
                                              {tr("Camera"), ui->cameraTab},
                                              {tr("Debug"), ui->debugTab},
                                              {tr("Web"), ui->webTab}};

    ui->tabWidget->clear();

    QStringList tabs = items[0]->data(Qt::UserRole).toStringList();

    for (const auto& tab : tabs)
        ui->tabWidget->addTab(widgets[tab], tab);
}
