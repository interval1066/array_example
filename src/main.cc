#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>
#include <memory>
#include <iterator>
#include <stdexcept>

static constexpr unsigned mymax = 256;

using namespace std;

// Useless ancestor class, but could be used to enforce an api policy or something.
template <class T>
class MemMgrString
{
public:
    virtual void InsertBlock(const string&) = 0;
    virtual void DisplayAll(void) = 0;
    virtual const unique_ptr<array<T, mymax>> GetBlock(const unsigned n) = 0;

    virtual void RemoveBlock(const unsigned) = 0;
};

template <class T>
class MemMgr : public MemMgrString<T>
{
    unique_ptr<vector<array<T, mymax>>> _vec;
    array<T, mymax> _block;

public:
    MemMgr<T>() : _vec(make_unique<vector<array<T, mymax>>>()) {}
    void InsertBlock(const string&);

    const unique_ptr<array<T, mymax>> GetBlock(const unsigned);
    void DisplayAll(void);
    void RemoveBlock(const unsigned);

    unsigned getSize(void) const { return _vec->size(); }
};

template <class T>
void
MemMgr<T>::InsertBlock(const string& str)
{
    if(_vec->size() > mymax)
        throw runtime_error("No space left");

    if(str.length() > mymax)
        throw runtime_error("Input string to long");

    fill(_block.begin(), _block.end(), 0);
    copy(str.begin(), str.end(), _block.data());
    _vec->push_back(_block);
}

template <class T>
const unique_ptr<array<T, mymax>>
MemMgr<T>::GetBlock(const unsigned n)
{
    if(_vec->size() < 1)
        throw runtime_error("Manager empty");

    auto it = next(_vec->begin(), n);
    fill(_block.begin(), _block.end(), 0);
    copy(it->begin(), it->end(), _block.data());

    return make_unique<array<T, mymax>>(_block);
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
    typename vector<array<T, mymax>>::const_iterator it;
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
    array<uint8_t, mymax> c;
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

        const auto& block = m->GetBlock(2);
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
