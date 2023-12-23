export struct xlColor {
    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;
};

struct HSVValue {
    float hue;
    float saturation;
    float value;
};

enum Pinwheel3DType {
    PW_3D_NONE,
    PW_3D,
    PW_3D_Inverted,
    PW_SWEEP
};

xlColor fromHSV(HSVValue hsv) {
    float red, green, blue;

    if (0.0f == hsv.saturation) {
        // Grey
        red = hsv.value;
        green = hsv.value;
        blue = hsv.value;
    } else { // not grey
        float hue = hsv.hue * 6.0;      // sector 0 to 5
        int i = (int)floor(hue);
        float f = hue - i;          // fractional part of h
        float p = hsv.value * (1.0 - hsv.saturation);

        switch (i) {
            case 0:
                red = hsv.value;
                green = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                blue = p;
                break;

            case 1:
                red = hsv.value * (1.0 - hsv.saturation * f);
                green = hsv.value;
                blue = p;
                break;

            case 2:
                red = p;
                green = hsv.value;
                blue = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                break;

            case 3:
                red = p;
                green = hsv.value * (1.0 - hsv.saturation * f);
                blue = hsv.value;
                break;

            case 4:
                red = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                green = p;
                blue = hsv.value;
                break;

            default:    // case 5:
                red = hsv.value;
                green = p;
                blue = hsv.value * (1.0 - hsv.saturation * f);
                break;
        }
    }
    xlColor result = {(uint8)(red * 255.0), (uint8)(green * 255.0), (uint8)(blue * 255.0)};
    return result;
}


export void pinwheelEffectKernel(uniform xlColor outputColumn[], uniform uint8 outputColumnIsSet[], uniform int bufferHt, uniform int x, uniform int x1, uniform int xc_adj, uniform int yc_adj, uniform int max_radius,
uniform int pinwheel_twist, uniform int pinwheel_rotation, uniform int pinwheel_arms, uniform int poffset, uniform int degrees_per_arm, uniform float pos, uniform Pinwheel3DType pw3dType, uniform float tmax, uniform bool bufferAllowAlpha,
uniform const xlColor colorsAsColor[]) {
    uniform float fX1 = (float)x1;

    foreach (y = 0...bufferHt) {
        int y1 = y - yc_adj - (bufferHt / 2);
        float fY1 = (float)y1;

        float r = sqrt((fX1*fX1)+(fY1*fY1));
        if (r <= max_radius) {
            float degrees_twist = (r / max_radius) * pinwheel_twist;
            float theta = (atan2(fX1, fY1) * 180 / 3.14159) + degrees_twist;
            if (pinwheel_rotation == 1) { // do we have CW rotation
                theta = pos + theta + (tmax / 2) + poffset;
            } else {
                theta = pos - theta + (tmax / 2) + poffset;
            }
            theta = theta + 540.0;
            int t2 = (int)theta % degrees_per_arm;

            outputColumnIsSet[y] = t2 <= tmax;
            if (t2 <= tmax) {
                float round = (float)t2 / (float)tmax;
                t2 = abs(t2 - (tmax / 2)) * 2;
                int ColorIdx2 = ((int)((theta / degrees_per_arm))) % pinwheel_arms;
                xlColor color = colorsAsColor[ColorIdx2];
                HSVValue hsv;
                // TODO: Use cif and branch back to a function pointer (supplied as an arg to this kernel) that can fetch the spatial colour
                //if (colorIsSpacial[ColorIdx2]) {
                //    buffer.palette.GetSpatialColor(colorarray[ColorIdx2], xc_adj + buffer.BufferWi / 2, yc_adj + bufferHt / 2, x, y, round, max_radius, color);
                //    if (!bufferAllowAlpha) {
                //        hsv = color.asHSV();
                //    }
                // TODO: Add colorsAsHSV argument to support this
                //} else if (!bufferAllowAlpha) {
                //    hsv = colorsAsHSV[ColorIdx2];
                //}
                switch (pw3dType) {
                case PW_3D:
                    if (bufferAllowAlpha) {
                        color.alpha = 255.0 * ((tmax - t2) / tmax);
                    } else {
                        hsv.value = hsv.value * ((tmax - t2) / tmax);
                        color = fromHSV(hsv);
                    }
                    break;
                case PW_3D_Inverted:
                    if (bufferAllowAlpha) {
                        color.alpha = 255.0 * ((t2) / tmax);
                    } else {
                        hsv.value = hsv.value * ((t2) / tmax);
                        color = fromHSV(hsv);
                    }
                    break;
                case PW_SWEEP:
                    if (bufferAllowAlpha) {
                        color.alpha = 255.0 * (1.0 - round);
                    } else {
                        hsv.value = hsv.value * (1.0 - round);
                        color = fromHSV(hsv);
                    }
                    break;
                default:
                    break;
                }
                outputColumn[y] = color;
            }
        }
    }
}
