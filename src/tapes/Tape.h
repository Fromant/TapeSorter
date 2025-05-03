#ifndef TAPE_H
#define TAPE_H

class Tape {
public:
    virtual ~Tape() = default;
    virtual int read() = 0;
    virtual void write(int value) = 0;
    virtual void shiftRight() = 0;
    virtual void shiftLeft() = 0;
    virtual void rewind() = 0;
    virtual bool hasNext() = 0;
    virtual int getPosition() = 0;
};


#endif //TAPE_H
