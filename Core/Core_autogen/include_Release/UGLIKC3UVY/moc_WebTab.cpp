/****************************************************************************
** Meta object code from reading C++ file 'WebTab.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Web/Tab/WebTab.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WebTab.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__WebTab_t {
    QByteArrayData data[14];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__WebTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__WebTab_t qt_meta_stringdata_Sn__WebTab = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Sn::WebTab"
QT_MOC_LITERAL(1, 11, 10), // "sNewWindow"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 7), // "sNewTab"
QT_MOC_LITERAL(4, 31, 7), // "sGoHome"
QT_MOC_LITERAL(5, 39, 13), // "showInspector"
QT_MOC_LITERAL(6, 53, 16), // "showNotification"
QT_MOC_LITERAL(7, 70, 8), // "QWidget*"
QT_MOC_LITERAL(8, 79, 5), // "notif"
QT_MOC_LITERAL(9, 85, 11), // "loadStarted"
QT_MOC_LITERAL(10, 97, 12), // "loadFinished"
QT_MOC_LITERAL(11, 110, 12), // "titleChanged"
QT_MOC_LITERAL(12, 123, 5), // "title"
QT_MOC_LITERAL(13, 129, 8) // "sRestore"

    },
    "Sn::WebTab\0sNewWindow\0\0sNewTab\0sGoHome\0"
    "showInspector\0showNotification\0QWidget*\0"
    "notif\0loadStarted\0loadFinished\0"
    "titleChanged\0title\0sRestore"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__WebTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    1,   63,    2, 0x08 /* Private */,
       9,    0,   66,    2, 0x08 /* Private */,
      10,    0,   67,    2, 0x08 /* Private */,
      11,    1,   68,    2, 0x08 /* Private */,
      13,    0,   71,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,

       0        // eod
};

void Sn::WebTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebTab *_t = static_cast<WebTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sNewWindow(); break;
        case 1: _t->sNewTab(); break;
        case 2: _t->sGoHome(); break;
        case 3: _t->showInspector(); break;
        case 4: _t->showNotification((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 5: _t->loadStarted(); break;
        case 6: _t->loadFinished(); break;
        case 7: _t->titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->sRestore(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::WebTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__WebTab.data,
      qt_meta_data_Sn__WebTab,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::WebTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::WebTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__WebTab.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::WebTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
