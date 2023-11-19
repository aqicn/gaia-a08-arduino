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

const uint32_t aqi_colors[6] = {0x00ff00, 0xffff00, 0xff8000, 0xff4000, 0xff0000, 0xcc00ff};

uint32_t pm25_concentration_to_color(float c)
{
    if (c >= 0 && c < 12.1)
        return aqi_colors[0];
    if (c >= 12.1 && c < 35.5)
        return aqi_colors[1];
    if (c >= 35.5 && c < 55.5)
        return aqi_colors[2];
    if (c >= 55.5 && c < 150.5)
        return aqi_colors[3];
    if (c >= 150.5 && c < 250.5)
        return aqi_colors[4];
    return aqi_colors[5];
}