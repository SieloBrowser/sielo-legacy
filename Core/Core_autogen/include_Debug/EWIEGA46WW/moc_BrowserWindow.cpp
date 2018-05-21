/****************************************************************************
** Meta object code from reading C++ file 'BrowserWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../BrowserWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BrowserWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__BrowserWindow_t {
    QByteArrayData data[25];
    char stringdata0[330];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__BrowserWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__BrowserWindow_t qt_meta_stringdata_Sn__BrowserWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Sn::BrowserWindow"
QT_MOC_LITERAL(1, 18, 14), // "setWindowTitle"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 5), // "title"
QT_MOC_LITERAL(4, 40, 19), // "enterHtmlFullScreen"
QT_MOC_LITERAL(5, 60, 16), // "toggleFullScreen"
QT_MOC_LITERAL(6, 77, 19), // "tabsSpaceInFullView"
QT_MOC_LITERAL(7, 97, 8), // "QWidget*"
QT_MOC_LITERAL(8, 106, 6), // "widget"
QT_MOC_LITERAL(9, 113, 17), // "arrangeTabsSpaces"
QT_MOC_LITERAL(10, 131, 15), // "bookmarkAllTabs"
QT_MOC_LITERAL(11, 147, 6), // "addTab"
QT_MOC_LITERAL(12, 154, 10), // "postLaunch"
QT_MOC_LITERAL(13, 165, 21), // "tabWidgetIndexChanged"
QT_MOC_LITERAL(14, 187, 10), // "TabWidget*"
QT_MOC_LITERAL(15, 198, 8), // "tbWidget"
QT_MOC_LITERAL(16, 207, 27), // "floatingButtonPatternChange"
QT_MOC_LITERAL(17, 235, 27), // "RootFloatingButton::Pattern"
QT_MOC_LITERAL(18, 263, 7), // "pattern"
QT_MOC_LITERAL(19, 271, 9), // "newWindow"
QT_MOC_LITERAL(20, 281, 6), // "goHome"
QT_MOC_LITERAL(21, 288, 7), // "forward"
QT_MOC_LITERAL(22, 296, 4), // "back"
QT_MOC_LITERAL(23, 301, 6), // "newTab"
QT_MOC_LITERAL(24, 308, 21) // "openAddBookmarkDialog"

    },
    "Sn::BrowserWindow\0setWindowTitle\0\0"
    "title\0enterHtmlFullScreen\0toggleFullScreen\0"
    "tabsSpaceInFullView\0QWidget*\0widget\0"
    "arrangeTabsSpaces\0bookmarkAllTabs\0"
    "addTab\0postLaunch\0tabWidgetIndexChanged\0"
    "TabWidget*\0tbWidget\0floatingButtonPatternChange\0"
    "RootFloatingButton::Pattern\0pattern\0"
    "newWindow\0goHome\0forward\0back\0newTab\0"
    "openAddBookmarkDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__BrowserWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x0a /* Public */,
       4,    0,   97,    2, 0x0a /* Public */,
       5,    0,   98,    2, 0x0a /* Public */,
       6,    1,   99,    2, 0x0a /* Public */,
       9,    0,  102,    2, 0x0a /* Public */,
      10,    0,  103,    2, 0x0a /* Public */,
      11,    0,  104,    2, 0x08 /* Private */,
      12,    0,  105,    2, 0x08 /* Private */,
      13,    1,  106,    2, 0x08 /* Private */,
      16,    1,  109,    2, 0x08 /* Private */,
      19,    0,  112,    2, 0x08 /* Private */,
      20,    0,  113,    2, 0x08 /* Private */,
      21,    0,  114,    2, 0x08 /* Private */,
      22,    0,  115,    2, 0x08 /* Private */,
      23,    0,  116,    2, 0x08 /* Private */,
      24,    0,  117,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::BrowserWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BrowserWindow *_t = static_cast<BrowserWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setWindowTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->enterHtmlFullScreen(); break;
        case 2: _t->toggleFullScreen(); break;
        case 3: _t->tabsSpaceInFullView((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 4: _t->arrangeTabsSpaces(); break;
        case 5: _t->bookmarkAllTabs(); break;
        case 6: _t->addTab(); break;
        case 7: _t->postLaunch(); break;
        case 8: _t->tabWidgetIndexChanged((*reinterpret_cast< TabWidget*(*)>(_a[1]))); break;
        case 9: _t->floatingButtonPatternChange((*reinterpret_cast< RootFloatingButton::Pattern(*)>(_a[1]))); break;
        case 10: _t->newWindow(); break;
        case 11: _t->goHome(); break;
        case 12: _t->forward(); break;
        case 13: _t->back(); break;
        case 14: _t->newTab(); break;
        case 15: _t->openAddBookmarkDialog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::BrowserWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Sn__BrowserWindow.data,
      qt_meta_data_Sn__BrowserWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::BrowserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::BrowserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__BrowserWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Sn::BrowserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
