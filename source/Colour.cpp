#include "Colour.hpp"

Colour::Colour() : red(0), green(0), blue(0), alpha(0) {
}

double Colour::Distance(const Colour& other) {
    
    // Note: Omitting alpha

    int R, G, B, A;

    R = (other.red - red);
    G = (other.green - green);
    B = (other.blue - blue);

    return R*R + G*G + B*B;
}

bool operator< (const Colour& x, const Colour& y) {
    
    Colour z; // Origin: zero vector Colour
    
    // Compute sum of the squared differences...
    double a = z.Distance(x);
    double b = z.Distance(y);
    
    // Compare...
    return (a < b);
}

bool operator== (const Colour& x, const Colour& y) {
    
    // Note: Omitting alpha
    
    if (x.red != y.red) return false;
    if (x.green != y.green) return false;
    if (x.blue != y.blue) return false;
    
    return true;
}
