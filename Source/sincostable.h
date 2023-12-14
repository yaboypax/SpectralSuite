#pragma once

/*
sincos table

History


This is completely free software
*/
#include "MathConstants.h"
#include "cplxd.h"

//******************************************************************************************
template <int BITS_>
class SinCosTable
    //
    // sin cos table template (length is 1<<BITS)
    // create table using "std::polar"
    // overflows of 1 or -1 are ok
    //
{
public:
    enum {
        BITS = BITS_,    // number of bits to index table
        LEN = 1 << BITS, // number of elements in table
        IMASK = LEN - 1  // index mask
    };
    static int size() { return LEN; }

    SinCosTable() { m_table = table_(); }

    // safe for any index
    template <class T> cplxd operator[](T i) const { return m_table[i & IMASK]; }

    // direct access, no index mask
    cplxd operator()(int i) const { return m_table[i]; }

protected:
    cplxd* m_table; // cached table pointer

    static cplxd* table_()
    {
        static bool init_table = true;
        static cplxd ext_table[LEN + 2];
        cplxd* table = ext_table + 1;
        if (init_table) {
            init_table = false;
            for (long i = 0; i < LEN; i++)
                table[i] = std::polar((double)1.0, (double)(i * 2.0 * 3.14159265358979323846 / LEN));

            // allow overflows of 1 or -1
            table[LEN] = table[0];
            table[-1] = table[LEN - 1];
        }
        return table;
    }
};

