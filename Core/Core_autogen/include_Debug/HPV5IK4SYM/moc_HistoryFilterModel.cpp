/****************************************************************************
** Meta object code from reading C++ file 'HistoryFilterModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../History/HistoryFilterModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HistoryFilterModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__HistoryFilterModel_t {
    QByteArrayData data[12];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__HistoryFilterModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__HistoryFilterModel_t qt_meta_stringdata_Sn__HistoryFilterModel = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Sn::HistoryFilterModel"
QT_MOC_LITERAL(1, 23, 11), // "sourceReset"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 17), // "sourceDataChanged"
QT_MOC_LITERAL(4, 54, 7), // "topLeft"
QT_MOC_LITERAL(5, 62, 11), // "bottomRight"
QT_MOC_LITERAL(6, 74, 18), // "sourceRowsInserted"
QT_MOC_LITERAL(7, 93, 6), // "parent"
QT_MOC_LITERAL(8, 100, 5), // "start"
QT_MOC_LITERAL(9, 106, 3), // "end"
QT_MOC_LITERAL(10, 110, 17), // "sourceRowsRemoved"
QT_MOC_LITERAL(11, 128, 5) // "index"

    },
    "Sn::HistoryFilterModel\0sourceReset\0\0"
    "sourceDataChanged\0topLeft\0bottomRight\0"
    "sourceRowsInserted\0parent\0start\0end\0"
    "sourceRowsRemoved\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__HistoryFilterModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    2,   35,    2, 0x08 /* Private */,
       6,    3,   40,    2, 0x08 /* Private */,
      10,    3,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    4,    5,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    7,    8,    9,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   11,    8,    9,

       0        // eod
};

void Sn::HistoryFilterModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryFilterModel *_t = static_cast<HistoryFilterModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sourceReset(); break;
        case 1: _t->sourceDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 2: _t->sourceRowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->sourceRowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::HistoryFilterModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_Sn__HistoryFilterModel.data,
      qt_meta_data_Sn__HistoryFilterModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::HistoryFilterModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::HistoryFilterModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__HistoryFilterModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractProxyModel::qt_metacast(_clname);
}

int Sn::HistoryFilterModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
