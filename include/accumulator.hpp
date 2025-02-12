/**
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once
#include <math.h>

template <typename T>
class Accumulator
{
    T *vals;
    int count = 0;
    int zeros = 0;
    int len;
    bool sorted = false;

public:
    Accumulator(int _len = 60)
    {
        len = _len;
        vals = new T[len];
        reset();
    }

    ~Accumulator()
    {
        delete[] vals;
        vals = nullptr;
    }

    void copy(Accumulator &acc)
    {
        reset();
        for (int i = 0; i < acc.count; i++)
        {
            add(acc.vals[i]);
        }
    }

    bool hasData()
    {
        return count != 0 || zeros > 0;
    }

    int last()
    {
        return vals[count ? (count - 1) : (len - 1)];
    }

    void add(T val)
    {
        if (count == len - 1)
        {
            for (int i = 0; i < len - 1; i++)
            {
                vals[i] = vals[i + 1];
            }
            count--;
        }
        vals[count++] = val;
        sorted = false;
    }

    void reset()
    {
        count = 0;
        zeros = 0;
        for (int i = 0; i < len; i++)
        {
            vals[i] = 0;
        }
        sorted = false;
    }

    float stddev()
    {
        if (count == 0)
            return 0;

        T u = avg();
        if (u < 0)
            return -1;
        T t = 0;
        for (int i = 0; i < count; i++)
        {
            t += (vals[i] - u) * (vals[i] - u);
        }
        return sqrt(t) / count;
    }

    int nOutliers()
    {
        int outliers = 0;
        for (int i = 0; i < count; i++)
        {
            if (isOutlier(vals[i]))
            {
                outliers++;
            }
        }
        return outliers;
    }

    bool isOutlier(float val)
    {
        if (val > median() + iqr() * 1.5)
        {
            return true;
        }
        else if (val < median() - iqr() * 1.5)
        {
            return true;
        }
        return false;
    }

    int median()
    {
        if (count == 0)
            return 0;
        sort();
        return vals[count / 2];
    }

    float iqr()
    {
        // 7 -> Q1: 3, Q2: 1, Q3: 5
        // 8 -> Q1: 2, Q2: 4, Q3: 6
        // 9 -> Q1: 2, Q2: 4, Q3: 9
        // 5 -> Q1: 2, Q2: 1, Q3: 3

        //[25,30,32,36,5218,5609,4697]
        // Q1=36, Q2=30, Q3=5609

        //[25,30,32,36,5218]
        // Q1=32, Q2=30, Q3=36

        if (count == 0)
            return 0;
        sort();
        float v = vals[count * 3 / 4] - vals[count / 4];
        float mv = vals[count / 2] / 10.0;
        if (v < mv)
            v = mv;
        return v;
    }

    void sort()
    {
        if (sorted)
            return;
        sorted = true;
        for (int i = 0; i < count; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (vals[i] < vals[j])
                {
                    float t = vals[j];
                    vals[j] = vals[i];
                    vals[i] = t;
                }
            }
        }
    }

    float avg()
    {
        if (count == 0)
            return 0;

        float t = 0;
        for (int i = 0; i < count; i++)
        {
            t += vals[i];
        }
        return t / count;
    }

    float vmin()
    {
        if (count == 0)
            return 0;

        float t = vals[0];
        for (int i = 0; i < count; i++)
        {
            if (t > vals[i])
            {
                t = vals[i];
            }
        }
        return t;
    }

    float vmax()
    {
        if (count == 0)
            return 0;

        float t = vals[0];
        for (int i = 0; i < count; i++)
        {
            if (vals[i] > t)
            {
                t = vals[i];
            }
        }
        return t;
    }
};
