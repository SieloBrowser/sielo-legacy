#ifndef EXPRESSION_BSON_H_NDB
#define EXPRESSION_BSON_H_NDB

#include <ndb/expression/bson/native.hpp>
#include <ndb/expression/bson/type.hpp>
#include <ndb/expression/bson/code.hpp>

#include <bcon.h>
#include <string>
#include <vector>

namespace ndb
{
    template<class Expr>
    class bson_expression
    {
    public:
        static constexpr auto expr_category = expr_category_code::bson;

        constexpr bson_expression(const Expr& expr)
        {
            document_ = bson_new();
            expr.make(*this);
        }

        ~bson_expression()
        {
            bson_destroy(document_);
        }


        void document_begin(const char* key)
        {
            bson_t* child = bson_new();
            bson_append_document_begin(document_, key, -1, child);
            document_childs_.push_back(child);
        }

        void document_end()
        {
            bson_append_document_end(document_, document_childs_.back());
            document_childs_.pop_back();
        }

        void array_begin(const char* key)
        {
            bson_t* child = bson_new();
            bson_append_array_begin(document_, key, -1, child);
            bson_append_int32(child, "aze", -1, 9);
            array_childs_.push_back(child);
        }

        void array_end()
        {
            bson_append_array_end(document_, array_childs_.back());
            array_childs_.pop_back();
        }

        void append(const char* key, int value)
        {
            bson_append_int32(document_, key, -1, value);
        }

        const bson_t* document()
        {
            return document_;
        }

        std::string to_string()
        {
            char* str = bson_as_json(document_, NULL);
            std::string result{ str };
            bson_free(str);
            return result;
        }

    private:
        bson_t* document_;
        bson_t* current_document_;
        std::vector<bson_t*> document_childs_;
        std::vector<bson_t*> array_childs_;
    };
} // ndb

#endif // EXPRESSION_BSON_H_NDB
