/****************************************************************************
** Meta object code from reading C++ file 'TabBarScrollWidget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Widgets/Tab/TabBarScrollWidget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TabBarScrollWidget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__TabBarScrollWidget_t {
    QByteArrayData data[17];
    char stringdata0[215];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__TabBarScrollWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__TabBarScrollWidget_t qt_meta_stringdata_Sn__TabBarScrollWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Sn::TabBarScrollWidget"
QT_MOC_LITERAL(1, 23, 13), // "ensureVisible"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 5), // "index"
QT_MOC_LITERAL(4, 44, 7), // "xmargin"
QT_MOC_LITERAL(5, 52, 12), // "scrollToLeft"
QT_MOC_LITERAL(6, 65, 1), // "n"
QT_MOC_LITERAL(7, 67, 18), // "QEasingCurve::Type"
QT_MOC_LITERAL(8, 86, 4), // "type"
QT_MOC_LITERAL(9, 91, 13), // "scrollToRight"
QT_MOC_LITERAL(10, 105, 16), // "scrollToLeftEdge"
QT_MOC_LITERAL(11, 122, 17), // "scrollToRightEdge"
QT_MOC_LITERAL(12, 140, 11), // "setUpLayout"
QT_MOC_LITERAL(13, 152, 15), // "overflowChanged"
QT_MOC_LITERAL(14, 168, 10), // "overflowed"
QT_MOC_LITERAL(15, 179, 11), // "scrollStart"
QT_MOC_LITERAL(16, 191, 23) // "updateScrollButtonState"

    },
    "Sn::TabBarScrollWidget\0ensureVisible\0"
    "\0index\0xmargin\0scrollToLeft\0n\0"
    "QEasingCurve::Type\0type\0scrollToRight\0"
    "scrollToLeftEdge\0scrollToRightEdge\0"
    "setUpLayout\0overflowChanged\0overflowed\0"
    "scrollStart\0updateScrollButtonState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__TabBarScrollWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   89,    2, 0x0a /* Public */,
       1,    1,   94,    2, 0x2a /* Public | MethodCloned */,
       1,    0,   97,    2, 0x2a /* Public | MethodCloned */,
       5,    2,   98,    2, 0x0a /* Public */,
       5,    1,  103,    2, 0x2a /* Public | MethodCloned */,
       5,    0,  106,    2, 0x2a /* Public | MethodCloned */,
       9,    2,  107,    2, 0x0a /* Public */,
       9,    1,  112,    2, 0x2a /* Public | MethodCloned */,
       9,    0,  115,    2, 0x2a /* Public | MethodCloned */,
      10,    0,  116,    2, 0x0a /* Public */,
      11,    0,  117,    2, 0x0a /* Public */,
      12,    0,  118,    2, 0x0a /* Public */,
      13,    1,  119,    2, 0x08 /* Private */,
      15,    0,  122,    2, 0x08 /* Private */,
      16,    0,  123,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Sn::TabBarScrollWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabBarScrollWidget *_t = static_cast<TabBarScrollWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ensureVisible((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->ensureVisible((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->ensureVisible(); break;
        case 3: _t->scrollToLeft((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QEasingCurve::Type(*)>(_a[2]))); break;
        case 4: _t->scrollToLeft((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->scrollToLeft(); break;
        case 6: _t->scrollToRight((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QEasingCurve::Type(*)>(_a[2]))); break;
        case 7: _t->scrollToRight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->scrollToRight(); break;
        case 9: _t->scrollToLeftEdge(); break;
        case 10: _t->scrollToRightEdge(); break;
        case 11: _t->setUpLayout(); break;
        case 12: _t->overflowChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->scrollStart(); break;
        case 14: _t->updateScrollButtonState(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::TabBarScrollWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Sn__TabBarScrollWidget.data,
      qt_meta_data_Sn__TabBarScrollWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::TabBarScrollWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::TabBarScrollWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__TabBarScrollWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Sn::TabBarScrollWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
