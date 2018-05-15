/****************************************************************************
** Meta object code from reading C++ file 'BookmarkManager.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Bookmarks/BookmarkManager.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BookmarkManager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__BookmarksManager_t {
    QByteArrayData data[13];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__BookmarksManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__BookmarksManager_t qt_meta_stringdata_Sn__BookmarksManager = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Sn::BookmarksManager"
QT_MOC_LITERAL(1, 21, 10), // "entryAdded"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 13), // "BookmarkNode*"
QT_MOC_LITERAL(4, 47, 4), // "item"
QT_MOC_LITERAL(5, 52, 12), // "entryRemoved"
QT_MOC_LITERAL(6, 65, 6), // "parent"
QT_MOC_LITERAL(7, 72, 3), // "row"
QT_MOC_LITERAL(8, 76, 12), // "entryChanged"
QT_MOC_LITERAL(9, 89, 15), // "importBookmarks"
QT_MOC_LITERAL(10, 105, 15), // "exportBookmarks"
QT_MOC_LITERAL(11, 121, 13), // "showBookmarks"
QT_MOC_LITERAL(12, 135, 4) // "save"

    },
    "Sn::BookmarksManager\0entryAdded\0\0"
    "BookmarkNode*\0item\0entryRemoved\0parent\0"
    "row\0entryChanged\0importBookmarks\0"
    "exportBookmarks\0showBookmarks\0save"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__BookmarksManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    3,   52,    2, 0x06 /* Public */,
       8,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   62,    2, 0x0a /* Public */,
      10,    0,   63,    2, 0x0a /* Public */,
      11,    0,   64,    2, 0x0a /* Public */,
      12,    0,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, 0x80000000 | 3,    6,    7,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::BookmarksManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksManager *_t = static_cast<BookmarksManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->entryAdded((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        case 1: _t->entryRemoved((*reinterpret_cast< BookmarkNode*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< BookmarkNode*(*)>(_a[3]))); break;
        case 2: _t->entryChanged((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        case 3: _t->importBookmarks(); break;
        case 4: _t->exportBookmarks(); break;
        case 5: _t->showBookmarks(); break;
        case 6: _t->save(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookmarksManager::entryAdded)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * , int , BookmarkNode * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookmarksManager::entryRemoved)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookmarksManager::entryChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::BookmarksManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Sn__BookmarksManager.data,
      qt_meta_data_Sn__BookmarksManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::BookmarksManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::BookmarksManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__BookmarksManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Sn::BookmarksManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void Sn::BookmarksManager::entryAdded(BookmarkNode * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::BookmarksManager::entryRemoved(BookmarkNode * _t1, int _t2, BookmarkNode * _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Sn::BookmarksManager::entryChanged(BookmarkNode * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
