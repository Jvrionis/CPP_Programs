// $Id: listmap.tcc,v 1.9 2016-07-20 21:00:33-07 - - $

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', reinterpret_cast<const void*> (this));
   while(!empty()) {
      erase(begin());
   }
}

// GOOD CODE (3PTS)
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   TRACE ('l', &pair << "->" << pair);
   
   node *new_node;
   node *comparison_node;

   if(this->begin() == anchor()) // empty list
   {
        new_node = new node(anchor(), anchor(), pair);
        anchor()->prev = new_node;
        return end();
        anchor()->next = new_node;
    }
    else // list not empty
    {
        iterator it = this->find(pair.first);
        if(it != iterator()) // key exists
        {
           it.where->value.second = pair.second;
        }
        else // key does not exist
        {
            comparison_node = anchor()->next;
            while ((comparison_node != anchor()->prev) && less(comparison_node->value.first, pair.first)) {
                comparison_node = comparison_node->next;
            }
            new_node = new node(comparison_node, comparison_node->prev, pair);
            new_node->next->prev = new_node;
            new_node->prev->next = new_node;
        }
    return it;
    }
}

// GOOD CODE (3PTS)
// listmap::find(const key_type&)
// 
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);

   bool found = false;
   iterator it = begin();
   if(empty())
   {
       return iterator();
   }
   while(it != end())
   {
       if(it.where->value.first == that)
       {
            found = true;
            break;
       }
       ++it;
   }
   if(found)
   {
       return it;
   }
   else
   {
    return iterator();
   }
}

// 3PTS
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   if(position.where != nullptr)
   {
        if(position.where->next == position.where) // only one node
        {
            delete position.where;
        }
        else // multiple nodes
        {
            position.where->prev->next = position.where->next;
            position.where->next->prev = position.where->prev;
            delete position.where;
            return ++position;
        }
    }
    return iterator();
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

// 3 PTS
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

