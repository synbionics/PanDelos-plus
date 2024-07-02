#include "MinBBHContainer.hh"
#include "../../threads/ThreadPool.hh"


using namespace bbh;
using namespace threads;

int main() {
    MinBBHContainer mBBH;

    // mBBH.resize(6);
    // mBBH.print();
    // mBBH.setVal(0, 2, 0.5);
    // mBBH.setVal(1, 3, 0.3);
    // mBBH.setVal(3, 4, 0.8);
    // mBBH.setVal(4, 5, 0.7);

    // mBBH.print();

    // mBBH.computeMins();
    // mBBH.print();


    ThreadPool tp;
    tp.start();
    mBBH.resize(3);

    mBBH.print();
    mBBH.setVal(0, 1, 0.3);
    mBBH.setVal(0, 2, 0.2);
    mBBH.setVal(1, 2, 0.1);

    mBBH.computeMins(tp);
    mBBH.print(); // 0.2 | 0.1 | 0.1
    tp.stop();


    return 0;
}