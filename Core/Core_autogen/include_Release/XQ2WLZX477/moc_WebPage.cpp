/****************************************************************************
** Meta object code from reading C++ file 'WebPage.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Web/WebPage.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WebPage.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Sn__WebPage_t {
    QByteArrayData data[17];
    char stringdata0[211];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sn__WebPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sn__WebPage_t qt_meta_stringdata_Sn__WebPage = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Sn::WebPage"
QT_MOC_LITERAL(1, 12, 14), // "privacyChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 6), // "status"
QT_MOC_LITERAL(4, 35, 8), // "progress"
QT_MOC_LITERAL(5, 44, 11), // "progression"
QT_MOC_LITERAL(6, 56, 8), // "finished"
QT_MOC_LITERAL(7, 65, 18), // "cleanBlockedObject"
QT_MOC_LITERAL(8, 84, 10), // "urlChanged"
QT_MOC_LITERAL(9, 95, 3), // "url"
QT_MOC_LITERAL(10, 99, 18), // "watchedFileChanged"
QT_MOC_LITERAL(11, 118, 4), // "file"
QT_MOC_LITERAL(12, 123, 20), // "windowCloseRequested"
QT_MOC_LITERAL(13, 144, 26), // "featurePermissionRequested"
QT_MOC_LITERAL(14, 171, 7), // "origine"
QT_MOC_LITERAL(15, 179, 23), // "QWebEnginePage::Feature"
QT_MOC_LITERAL(16, 203, 7) // "feature"

    },
    "Sn::WebPage\0privacyChanged\0\0status\0"
    "progress\0progression\0finished\0"
    "cleanBlockedObject\0urlChanged\0url\0"
    "watchedFileChanged\0file\0windowCloseRequested\0"
    "featurePermissionRequested\0origine\0"
    "QWebEnginePage::Feature\0feature"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sn__WebPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   57,    2, 0x09 /* Protected */,
       6,    0,   60,    2, 0x09 /* Protected */,
       7,    0,   61,    2, 0x08 /* Private */,
       8,    1,   62,    2, 0x08 /* Private */,
      10,    1,   65,    2, 0x08 /* Private */,
      12,    0,   68,    2, 0x08 /* Private */,
      13,    2,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 15,   14,   16,

       0        // eod
};

void Sn::WebPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebPage *_t = static_cast<WebPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->privacyChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->progress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->finished(); break;
        case 3: _t->cleanBlockedObject(); break;
        case 4: _t->urlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 5: _t->watchedFileChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->windowCloseRequested(); break;
        case 7: _t->featurePermissionRequested((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< const QWebEnginePage::Feature(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (WebPage::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WebPage::privacyChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Sn::WebPage::staticMetaObject = {
    { &QWebEnginePage::staticMetaObject, qt_meta_stringdata_Sn__WebPage.data,
      qt_meta_data_Sn__WebPage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Sn::WebPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sn::WebPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Sn__WebPage.stringdata0))
        return static_cast<void*>(this);
    return QWebEnginePage::qt_metacast(_clname);
}

int Sn::WebPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEnginePage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Sn::WebPage::privacyChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
