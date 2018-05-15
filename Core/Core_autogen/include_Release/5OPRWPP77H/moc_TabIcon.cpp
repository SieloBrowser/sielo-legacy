/****************************************************************************
** Meta object code from reading C++ file 'TabIcon.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Tab/TabIcon.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TabIcon.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__TabIcon_t {
    QByteArrayData data[8];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__TabIcon_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__TabIcon_t qt_meta_stringdata_Sn__TabIcon = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Sn::TabIcon"
QT_MOC_LITERAL(1, 12, 7), // "resized"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 20), // "showLoadingAnimation"
QT_MOC_LITERAL(4, 42, 20), // "hideLoadingAnimation"
QT_MOC_LITERAL(5, 63, 15), // "updateAudioIcon"
QT_MOC_LITERAL(6, 79, 15), // "recentlyAudible"
QT_MOC_LITERAL(7, 95, 20) // "updateAnimationFrame"

    },
    "Sn::TabIcon\0resized\0\0showLoadingAnimation\0"
    "hideLoadingAnimation\0updateAudioIcon\0"
    "recentlyAudible\0updateAnimationFrame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__TabIcon[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    1,   42,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,

       0        // eod
};

void Sn::TabIcon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabIcon *_t = static_cast<TabIcon *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resized(); break;
        case 1: _t->showLoadingAnimation(); break;
        case 2: _t->hideLoadingAnimation(); break;
        case 3: _t->updateAudioIcon((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->updateAnimationFrame(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (TabIcon::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabIcon::resized)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::TabIcon::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__TabIcon.data,
      qt_meta_data_Sn__TabIcon,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::TabIcon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::TabIcon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__TabIcon.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::TabIcon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Sn::TabIcon::resized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
