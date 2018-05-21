/****************************************************************************
** Meta object code from reading C++ file 'TabbedWebView.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Web/Tab/TabbedWebView.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TabbedWebView.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__TabbedWebView_t {
    QByteArrayData data[19];
    char stringdata0[197];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__TabbedWebView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__TabbedWebView_t qt_meta_stringdata_Sn__TabbedWebView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Sn::TabbedWebView"
QT_MOC_LITERAL(1, 18, 13), // "wantsCloseTab"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "ipChanged"
QT_MOC_LITERAL(4, 43, 15), // "setAsCurrentTab"
QT_MOC_LITERAL(5, 59, 14), // "userLoadAction"
QT_MOC_LITERAL(6, 74, 11), // "LoadRequest"
QT_MOC_LITERAL(7, 86, 7), // "request"
QT_MOC_LITERAL(8, 94, 12), // "sLoadStarted"
QT_MOC_LITERAL(9, 107, 13), // "sLoadProgress"
QT_MOC_LITERAL(10, 121, 8), // "progress"
QT_MOC_LITERAL(11, 130, 13), // "sLoadFinished"
QT_MOC_LITERAL(12, 144, 10), // "urlChanged"
QT_MOC_LITERAL(13, 155, 3), // "url"
QT_MOC_LITERAL(14, 159, 11), // "linkHovered"
QT_MOC_LITERAL(15, 171, 4), // "link"
QT_MOC_LITERAL(16, 176, 5), // "setIp"
QT_MOC_LITERAL(17, 182, 9), // "QHostInfo"
QT_MOC_LITERAL(18, 192, 4) // "info"

    },
    "Sn::TabbedWebView\0wantsCloseTab\0\0"
    "ipChanged\0setAsCurrentTab\0userLoadAction\0"
    "LoadRequest\0request\0sLoadStarted\0"
    "sLoadProgress\0progress\0sLoadFinished\0"
    "urlChanged\0url\0linkHovered\0link\0setIp\0"
    "QHostInfo\0info"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__TabbedWebView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       3,    1,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   70,    2, 0x0a /* Public */,
       5,    1,   71,    2, 0x0a /* Public */,
       8,    0,   74,    2, 0x08 /* Private */,
       9,    1,   75,    2, 0x08 /* Private */,
      11,    0,   78,    2, 0x08 /* Private */,
      12,    1,   79,    2, 0x08 /* Private */,
      14,    1,   82,    2, 0x08 /* Private */,
      16,    1,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,   13,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

void Sn::TabbedWebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabbedWebView *_t = static_cast<TabbedWebView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wantsCloseTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->ipChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setAsCurrentTab(); break;
        case 3: _t->userLoadAction((*reinterpret_cast< const LoadRequest(*)>(_a[1]))); break;
        case 4: _t->sLoadStarted(); break;
        case 5: _t->sLoadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->sLoadFinished(); break;
        case 7: _t->urlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 8: _t->linkHovered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->setIp((*reinterpret_cast< const QHostInfo(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QHostInfo >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (TabbedWebView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabbedWebView::wantsCloseTab)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TabbedWebView::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabbedWebView::ipChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::TabbedWebView::staticMetaObject = {
    { &WebView::staticMetaObject, qt_meta_stringdata_Sn__TabbedWebView.data,
      qt_meta_data_Sn__TabbedWebView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::TabbedWebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::TabbedWebView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__TabbedWebView.stringdata0))
        return static_cast<void*>(this);
    return WebView::qt_metacast(_clname);
}

int Sn::TabbedWebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WebView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Sn::TabbedWebView::wantsCloseTab(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Sn::TabbedWebView::ipChanged(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
