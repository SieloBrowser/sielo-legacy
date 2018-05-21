/****************************************************************************
** Meta object code from reading C++ file 'ToolButton.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Utils/ToolButton.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ToolButton.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__ToolButton_t {
    QByteArrayData data[16];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__ToolButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__ToolButton_t qt_meta_stringdata_Sn__ToolButton = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Sn::ToolButton"
QT_MOC_LITERAL(1, 15, 18), // "middleMouseClicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 14), // "controlClicked"
QT_MOC_LITERAL(4, 50, 13), // "doubleClicked"
QT_MOC_LITERAL(5, 64, 15), // "aboutToShowMenu"
QT_MOC_LITERAL(6, 80, 15), // "aboutToHideMenu"
QT_MOC_LITERAL(7, 96, 15), // "menuAboutToHide"
QT_MOC_LITERAL(8, 112, 8), // "showMenu"
QT_MOC_LITERAL(9, 121, 9), // "fixedsize"
QT_MOC_LITERAL(10, 131, 10), // "fixedwidth"
QT_MOC_LITERAL(11, 142, 11), // "fixedheight"
QT_MOC_LITERAL(12, 154, 9), // "multiIcon"
QT_MOC_LITERAL(13, 164, 4), // "icon"
QT_MOC_LITERAL(14, 169, 9), // "themeIcon"
QT_MOC_LITERAL(15, 179, 12) // "fallbackIcon"

    },
    "Sn::ToolButton\0middleMouseClicked\0\0"
    "controlClicked\0doubleClicked\0"
    "aboutToShowMenu\0aboutToHideMenu\0"
    "menuAboutToHide\0showMenu\0fixedsize\0"
    "fixedwidth\0fixedheight\0multiIcon\0icon\0"
    "themeIcon\0fallbackIcon"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__ToolButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       7,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,
       6,    0,   53,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::QSize, 0x00095003,
      10, QMetaType::Int, 0x00095003,
      11, QMetaType::Int, 0x00095003,
      12, QMetaType::QImage, 0x00095103,
      13, QMetaType::QIcon, 0x00095103,
      14, QMetaType::QString, 0x00095103,
      15, QMetaType::QIcon, 0x00095103,

       0        // eod
};

void Sn::ToolButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ToolButton *_t = static_cast<ToolButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->middleMouseClicked(); break;
        case 1: _t->controlClicked(); break;
        case 2: _t->doubleClicked(); break;
        case 3: _t->aboutToShowMenu(); break;
        case 4: _t->aboutToHideMenu(); break;
        case 5: _t->menuAboutToHide(); break;
        case 6: _t->showMenu(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ToolButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolButton::middleMouseClicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ToolButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolButton::controlClicked)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ToolButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolButton::doubleClicked)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ToolButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolButton::aboutToShowMenu)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ToolButton::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolButton::aboutToHideMenu)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ToolButton *_t = static_cast<ToolButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QSize*>(_v) = _t->size(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->width(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->height(); break;
        case 3: *reinterpret_cast< QImage*>(_v) = _t->multiIcon(); break;
        case 4: *reinterpret_cast< QIcon*>(_v) = _t->icon(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->themeIcon(); break;
        case 6: *reinterpret_cast< QIcon*>(_v) = _t->fallbackIcon(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ToolButton *_t = static_cast<ToolButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFixedSize(*reinterpret_cast< QSize*>(_v)); break;
        case 1: _t->setFixedWidth(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setFixedHeight(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setMultiIcon(*reinterpret_cast< QImage*>(_v)); break;
        case 4: _t->setIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 5: _t->setThemeIcon(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setFallbackIcon(*reinterpret_cast< QIcon*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Sn::ToolButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_Sn__ToolButton.data,
      qt_meta_data_Sn__ToolButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::ToolButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::ToolButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__ToolButton.stringdata0))
        return static_cast<void*>(this);
    return QToolButton::qt_metacast(_clname);
}

int Sn::ToolButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Sn::ToolButton::middleMouseClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Sn::ToolButton::controlClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Sn::ToolButton::doubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Sn::ToolButton::aboutToShowMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Sn::ToolButton::aboutToHideMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
