#pragma once

#include "ITask.h"
#include "BMP library/EasyBMP.h"

#define _USE_MATH_DEFINES
#include <math.h>

struct BlurParams
{
    BMP& bmp;
    int startPoint;
    int endPoint;
};

class BlurTask : public ITask
{
private:
    int m_radius;
    BlurParams* m_params;
public:
    BlurTask(int radius, BlurParams* params)
        :m_radius(radius)
        , m_params(params)
    {
    }

    void Execute() override
    {
        int imageHeight = m_params->bmp.TellHeight();
        int imageWidth = m_params->bmp.TellWidth();
        float rs = ceil(m_radius * 2.57);
        for (int i = 0; i < imageHeight; i++)
        {
            for (int j = m_params->startPoint; j < m_params->endPoint; j++)
            {
                double wsum = 0;
                double r = 0, g = 0, b = 0;
                for (int iy = i - rs; iy < i + rs + 1; iy++)
                {
                    for (int ix = j - rs; ix < j + rs + 1; ix++)
                    {
                        auto x = std::min<int>(imageWidth - 1, std::max<int>(0, ix));
                        auto y = std::min<int>(imageHeight - 1, std::max<int>(0, iy));
                        auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                        auto wght = std::exp(-dsq / (2.0 * m_radius * m_radius)) / (M_PI * 2.0 * m_radius * m_radius);

                        RGBApixel pixel = m_params->bmp.GetPixel(x, y);
                        r += pixel.Red * wght;
                        g += pixel.Green * wght;
                        b += pixel.Blue * wght;

                        wsum += wght;
                    }
                }
                RGBApixel pixel = m_params->bmp.GetPixel(j, i);
                pixel.Red = std::round(r / wsum);
                pixel.Green = std::round(g / wsum);
                pixel.Blue = std::round(b / wsum);
                m_params->bmp.SetPixel(j, i, pixel);
            }
        }
    }
};