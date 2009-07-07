#ifndef PSEUDO_H_INCLUDED
#define PSEUDO_H_INCLUDED

class Pseudo
{
    public:
        Pseudo ();
        float x;
        float y;

        void set_pos(float x, float y);
        float get_pos();
        ~Pseudo();
};


#endif // PSEUDO_H_INCLUDED
