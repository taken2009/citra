// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <atomic>
#include <map>
#include <unordered_map>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QRunnable>
#include <QStandardItem>
#include <QString>
#include "citra_qt/util/util.h"
#include "common/string_util.h"
#include "core/loader/smdh.h"

/**
 * Gets the game icon from SMDH data.
 * @param smdh SMDH data
 * @param large If true, returns large icon (48x48), otherwise returns small icon (24x24)
 * @return QPixmap game icon
 */
static QPixmap GetQPixmapFromSMDH(const Loader::SMDH& smdh, bool large) {
    std::vector<u16> icon_data = smdh.GetIcon(large);
    const uchar* data = reinterpret_cast<const uchar*>(icon_data.data());
    int size = large ? 48 : 24;
    QImage icon(data, size, size, QImage::Format::Format_RGB16);
    return QPixmap::fromImage(icon);
}

/**
 * Gets the default icon (for games without valid SMDH)
 * @param large If true, returns large icon (48x48), otherwise returns small icon (24x24)
 * @return QPixmap default icon
 */
static QPixmap GetDefaultIcon(bool large) {
    int size = large ? 48 : 24;
    QPixmap icon(size, size);
    icon.fill(Qt::transparent);
    return icon;
}

/**
 * Creates a circle pixmap from a specified color
 * @param color The color the pixmap shall have
 * @return QPixmap circle pixmap
 */
static QPixmap CreateCirclePixmapFromColor(const QColor& color) {
    QPixmap circle_pixmap(16, 16);
    circle_pixmap.fill(Qt::transparent);

    QPainter painter(&circle_pixmap);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, 15, 15);

    return circle_pixmap;
}

/**
 * Gets the short game title from SMDH data.
 * @param smdh SMDH data
 * @param language title language
 * @return QString short title
 */
static QString GetQStringShortTitleFromSMDH(const Loader::SMDH& smdh,
                                            Loader::SMDH::TitleLanguage language) {
    return QString::fromUtf16(smdh.GetShortTitle(language).data());
}

class GameListItem : public QStandardItem {
public:
    GameListItem() : QStandardItem() {}
    GameListItem(const QString& string) : QStandardItem(string) {}
    virtual ~GameListItem() override {}
};

/**
 * A specialization of GameListItem for path values.
 * This class ensures that for every full path value it holds, a correct string representation
 * of just the filename (with no extension) will be displayed to the user.
 * If this class receives valid SMDH data, it will also display game icons and titles.
 */
class GameListItemPath : public GameListItem {
public:
    static const int FullPathRole = Qt::UserRole + 1;
    static const int TitleRole = Qt::UserRole + 2;
    static const int ProgramIdRole = Qt::UserRole + 3;

    GameListItemPath() : GameListItem() {}
    GameListItemPath(const QString& game_path, const std::vector<u8>& smdh_data, u64 program_id)
        : GameListItem() {
        setData(game_path, FullPathRole);
        setData(qulonglong(program_id), ProgramIdRole);

        if (!Loader::IsValidSMDH(smdh_data)) {
            // SMDH is not valid, set a default icon
            setData(GetDefaultIcon(true), Qt::DecorationRole);
            return;
        }

        Loader::SMDH smdh;
        memcpy(&smdh, smdh_data.data(), sizeof(Loader::SMDH));

        // Get icon from SMDH
        setData(GetQPixmapFromSMDH(smdh, true), Qt::DecorationRole);

        // Get title from SMDH
        setData(GetQStringShortTitleFromSMDH(smdh, Loader::SMDH::TitleLanguage::English),
                TitleRole);
    }

    QVariant data(int role) const override {
        if (role == Qt::DisplayRole) {
            std::string filename;
            Common::SplitPath(data(FullPathRole).toString().toStdString(), nullptr, &filename,
                              nullptr);
            QString title = data(TitleRole).toString();
            return QString::fromStdString(filename) + (title.isEmpty() ? "" : "\n    " + title);
        } else {
            return GameListItem::data(role);
        }
    }
};

class GameListItemCompat : public GameListItem {
public:
    static const int CompatNumberRole = Qt::UserRole + 1;
    GameListItemCompat() = default;
    explicit GameListItemCompat(const QString compatiblity) {
        Compat_Status status = status_data.find(compatiblity)->second;
        setData(compatiblity, CompatNumberRole);
        setText(status.text);
        setToolTip(status.tooltip);
        setData(CreateCirclePixmapFromColor(status.color), Qt::DecorationRole);
    }

    bool operator<(const QStandardItem& other) const override {
        return data(CompatNumberRole) < other.data(CompatNumberRole);
    }

private:
    struct Compat_Status {
        QString color;
        QString text;
        QString tooltip;
    };

    // clang-format off
    const static inline std::map<QString, Compat_Status> status_data = {
        { "0",{ "#5c93ed", QObject::tr("Perfect"), QObject::tr("Game functions flawless with no audio or graphical glitches, all tested functionality works as intended without\nany workarounds needed.") } },
    { "1",{ "#47d35c", QObject::tr("Great"), QObject::tr("Game functions with minor graphical or audio glitches and is playable from start to finish. May require some\nworkarounds.") } },
    { "2",{ "#94b242", QObject::tr("Okay"), QObject::tr("Game functions with major graphical or audio glitches, but game is playable from start to finish with\nworkarounds.") } },
    { "3",{ "#f2d624", QObject::tr("Bad"), QObject::tr("Game functions, but with major graphical or audio glitches. Unable to progress in specific areas due to glitches\neven with workarounds.") } },
    { "4",{ "#FF0000", QObject::tr("Intro/Menu"), QObject::tr("Game is completely unplayable due to major graphical or audio glitches. Unable to progress past the Start\nScreen.") } },
    { "5",{ "#828282", QObject::tr("Won't Boot"), QObject::tr("The game crashes when attempting to startup.") } },
    { "" ,{ "#000000", QObject::tr("Not Tested"), QObject::tr("The game has not yet been tested.") } },
    };
    // clang-format on
};

/**
 * A specialization of GameListItem for size values.
 * This class ensures that for every numerical size value it holds (in bytes), a correct
 * human-readable string representation will be displayed to the user.
 */
class GameListItemSize : public GameListItem {
public:
    static const int SizeRole = Qt::UserRole + 1;

    GameListItemSize() : GameListItem() {}
    GameListItemSize(const qulonglong size_bytes) : GameListItem() {
        setData(size_bytes, SizeRole);
    }

    void setData(const QVariant& value, int role) override {
        // By specializing setData for SizeRole, we can ensure that the numerical and string
        // representations of the data are always accurate and in the correct format.
        if (role == SizeRole) {
            qulonglong size_bytes = value.toULongLong();
            GameListItem::setData(ReadableByteSize(size_bytes), Qt::DisplayRole);
            GameListItem::setData(value, SizeRole);
        } else {
            GameListItem::setData(value, role);
        }
    }

    /**
     * This operator is, in practice, only used by the TreeView sorting systems.
     * Override it so that it will correctly sort by numerical value instead of by string
     * representation.
     */
    bool operator<(const QStandardItem& other) const override {
        return data(SizeRole).toULongLong() < other.data(SizeRole).toULongLong();
    }
};

/**
 * Asynchronous worker object for populating the game list.
 * Communicates with other threads through Qt's signal/slot system.
 */
class GameListWorker : public QObject, public QRunnable {
    Q_OBJECT

public:
    GameListWorker(QString dir_path, bool deep_scan,
                   std::unordered_map<std::string, QString>& compatibility_list)
        : QObject(), QRunnable(), dir_path(dir_path), deep_scan(deep_scan),
          compatibility_list(compatibility_list) {}

public slots:
    /// Starts the processing of directory tree information.
    void run() override;
    /// Tells the worker that it should no longer continue processing. Thread-safe.
    void Cancel();

signals:
    /**
     * The `EntryReady` signal is emitted once an entry has been prepared and is ready
     * to be added to the game list.
     * @param entry_items a list with `QStandardItem`s that make up the columns of the new entry.
     */
    void EntryReady(QList<QStandardItem*> entry_items);

    /**
     * After the worker has traversed the game directory looking for entries, this signal is emmited
     * with a list of folders that should be watched for changes as well.
     */
    void Finished(QStringList watch_list);

private:
    QStringList watch_list;
    QString dir_path;
    bool deep_scan;
    std::unordered_map<std::string, QString>& compatibility_list;
    std::atomic_bool stop_processing;

    void AddFstEntriesToGameList(const std::string& dir_path, unsigned int recursion = 0);
};
