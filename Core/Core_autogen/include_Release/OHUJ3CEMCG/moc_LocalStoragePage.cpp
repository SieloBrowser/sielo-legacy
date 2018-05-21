/****************************************************************************
** Meta object code from reading C++ file 'LocalStoragePage.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Preferences/LocalStoragePage.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LocalStoragePage.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__LocalStoragePage_t {
    QByteArrayData data[10];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__LocalStoragePage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__LocalStoragePage_t qt_meta_stringdata_Sn__LocalStoragePage = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Sn::LocalStoragePage"
QT_MOC_LITERAL(1, 21, 26), // "allowNetworkStorageChanged"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 5), // "allow"
QT_MOC_LITERAL(4, 55, 25), // "allowSavingHistoryChanged"
QT_MOC_LITERAL(5, 81, 24), // "allowHTML5StorageChanged"
QT_MOC_LITERAL(6, 106, 17), // "cacheValueChanged"
QT_MOC_LITERAL(7, 124, 5), // "value"
QT_MOC_LITERAL(8, 130, 28), // "changeStorageLocationClicked"
QT_MOC_LITERAL(9, 159, 25) // "deleteHTML5StorageClicked"

    },
    "Sn::LocalStoragePage\0allowNetworkStorageChanged\0"
    "\0allow\0allowSavingHistoryChanged\0"
    "allowHTML5StorageChanged\0cacheValueChanged\0"
    "value\0changeStorageLocationClicked\0"
    "deleteHTML5StorageClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__LocalStoragePage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       4,    1,   47,    2, 0x08 /* Private */,
       5,    1,   50,    2, 0x08 /* Private */,
       6,    1,   53,    2, 0x08 /* Private */,
       8,    0,   56,    2, 0x08 /* Private */,
       9,    0,   57,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::LocalStoragePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LocalStoragePage *_t = static_cast<LocalStoragePage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->allowNetworkStorageChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->allowSavingHistoryChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->allowHTML5StorageChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->cacheValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->changeStorageLocationClicked(); break;
        case 5: _t->deleteHTML5StorageClicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::LocalStoragePage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__LocalStoragePage.data,
      qt_meta_data_Sn__LocalStoragePage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::LocalStoragePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::LocalStoragePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__LocalStoragePage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::LocalStoragePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
