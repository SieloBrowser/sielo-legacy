/****************************************************************************
** Meta object code from reading C++ file 'HistoryManager.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../History/HistoryManager.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HistoryManager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__HistoryManager_t {
    QByteArrayData data[16];
    char stringdata0[186];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__HistoryManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__HistoryManager_t qt_meta_stringdata_Sn__HistoryManager = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Sn::HistoryManager"
QT_MOC_LITERAL(1, 19, 12), // "historyReset"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 10), // "entryAdded"
QT_MOC_LITERAL(4, 44, 11), // "HistoryItem"
QT_MOC_LITERAL(5, 56, 4), // "item"
QT_MOC_LITERAL(6, 61, 12), // "entryRemoved"
QT_MOC_LITERAL(7, 74, 11), // "entryUpdate"
QT_MOC_LITERAL(8, 86, 6), // "offset"
QT_MOC_LITERAL(9, 93, 5), // "clear"
QT_MOC_LITERAL(10, 99, 12), // "loadSettings"
QT_MOC_LITERAL(11, 112, 10), // "showDialog"
QT_MOC_LITERAL(12, 123, 4), // "save"
QT_MOC_LITERAL(13, 128, 15), // "checkForExpired"
QT_MOC_LITERAL(14, 144, 28), // "removeExpiredEntriesDirectly"
QT_MOC_LITERAL(15, 173, 12) // "historyLimit"

    },
    "Sn::HistoryManager\0historyReset\0\0"
    "entryAdded\0HistoryItem\0item\0entryRemoved\0"
    "entryUpdate\0offset\0clear\0loadSettings\0"
    "showDialog\0save\0checkForExpired\0"
    "removeExpiredEntriesDirectly\0historyLimit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__HistoryManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       1,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    1,   65,    2, 0x06 /* Public */,
       6,    1,   68,    2, 0x06 /* Public */,
       7,    1,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   74,    2, 0x0a /* Public */,
      10,    0,   75,    2, 0x0a /* Public */,
      11,    0,   76,    2, 0x0a /* Public */,
      12,    0,   77,    2, 0x08 /* Private */,
      13,    1,   78,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x28 /* Private | MethodCloned */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,

 // properties: name, type, flags
      15, QMetaType::Int, 0x00095103,

       0        // eod
};

void Sn::HistoryManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->historyReset(); break;
        case 1: _t->entryAdded((*reinterpret_cast< const HistoryItem(*)>(_a[1]))); break;
        case 2: _t->entryRemoved((*reinterpret_cast< const HistoryItem(*)>(_a[1]))); break;
        case 3: _t->entryUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->clear(); break;
        case 5: _t->loadSettings(); break;
        case 6: _t->showDialog(); break;
        case 7: _t->save(); break;
        case 8: _t->checkForExpired((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->checkForExpired(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (HistoryManager::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistoryManager::historyReset)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HistoryManager::*_t)(const HistoryItem & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistoryManager::entryAdded)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HistoryManager::*_t)(const HistoryItem & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistoryManager::entryRemoved)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (HistoryManager::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistoryManager::entryUpdate)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->historyLimit(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setHistoryLimit(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Sn::HistoryManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Sn__HistoryManager.data,
      qt_meta_data_Sn__HistoryManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::HistoryManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::HistoryManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__HistoryManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Sn::HistoryManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Sn::HistoryManager::historyReset()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Sn::HistoryManager::entryAdded(const HistoryItem & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Sn::HistoryManager::entryRemoved(const HistoryItem & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Sn::HistoryManager::entryUpdate(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
