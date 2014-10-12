#ifndef __COLOUR_HPP__
#define	__COLOUR_HPP__

class Colour {
    
    public:
        
        unsigned char red, green, blue;
        unsigned char alpha;
        
        Colour();
        
        virtual double Distance(const Colour& other);
        
        friend bool operator< (const Colour& x, const Colour& y);
        friend bool operator== (const Colour& x, const Colour& y);
};

extern bool operator< (const Colour& x, const Colour& y);
extern bool operator== (const Colour& x, const Colour& y);

#endif
