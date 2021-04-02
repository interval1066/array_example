#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>
#include <cstring>
#include <memory>
#include <iterator>
#include <stdexcept>

#define MAX 256

using namespace std;

template <class T>
class MemMgrString
{
public:
    virtual void InsertBlock(const string&) = 0;
    virtual void DisplayAll(void) = 0;
    virtual unique_ptr<array<T, MAX>> GetBlock(const unsigned n) = 0;

    virtual void RemoveBlock(const unsigned) = 0;
};

template <class T>
class MemMgr : public MemMgrString<T>
{
    unique_ptr<vector<array<T, MAX>>> _vec;
    array<T, MAX> _block;

public:
    MemMgr<T>();
    void InsertBlock(const string&);
    unique_ptr<array<T, MAX>> GetBlock(const unsigned);
    void DisplayAll(void);

    void RemoveBlock(const unsigned);
    unsigned getSize(void) const { return _vec->size(); }
};

template <class T>
MemMgr<T>::MemMgr() :
    _vec(make_unique<vector<array<T, MAX>>>())
{
}

template <class T>
void
MemMgr<T>::InsertBlock(const string& str)
{
    if(_vec->size() > MAX)
        throw runtime_error("No space left");

    memset(&_block, '\0', MAX);
    memcpy(&_block, str.c_str(), str.length());
    _vec->push_back(_block);
}

template <class T>
unique_ptr<array<T, MAX>>
MemMgr<T>::GetBlock(const unsigned n)
{
    if(_vec->size() < 1)
        throw runtime_error("Manager empty");

    auto it = next(_vec->begin(), n);
    memset(&_block, '\0', MAX);
    memcpy(&_block, it->data(), MAX);

    return make_unique<array<T, MAX>>(_block);
}

template <class T>
void
MemMgr<T>::RemoveBlock(const unsigned n)
{
    if(_vec->size() < 1)
        throw runtime_error("Manager empty");

    auto it = next(_vec->begin(), n);
    _vec->erase(it);
}

// For testing
template <class T>
void
MemMgr<T>::DisplayAll()
{
    string str;
    typename vector<array<T, MAX>>::const_iterator it;
    for(it = _vec->begin(); it != _vec->end(); ++it) {
        for(auto n : *it)
            str += n;

        cout << str.substr(0, str.find_first_of('\0')) << endl;
        str.clear();
    }
}

int
main(__attribute__((unused))int argc, __attribute__((unused))char** argv)
{
    array<uint8_t, MAX> c;
    string output;
    auto m = make_unique<MemMgr<uint8_t>>();

    try {
        string str = "Now is the time";
        m->InsertBlock(str);

        str = "for all good men";
        m->InsertBlock(str);

        str = "to come to the aid";
        m->InsertBlock(str);

        str = "of the party.";
        m->InsertBlock(str);

        auto block = m->GetBlock(2);
        copy(block->begin(), block->end(), c.begin());
        for(char value : c)
            output += value;

        cout << output.c_str() << endl;

        m->RemoveBlock(2);
        m->DisplayAll();
        cout << m->getSize() << endl;
    }
    catch(exception const& e)
    {
        cout << "Exception: " << e.what() << endl;
    }

    return EXIT_SUCCESS;
}
