//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setting.cpp


#include "keymap.h"
#include "errormessage.h"
#include "stringtool.h"
#include <algorithm>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Action

//
tostream &operator<<(tostream &i_ost, const Action &i_action)
{
  return i_action.output(i_ost);
}


//
ActionKey::ActionKey(const ModifiedKey &i_mk)
  : m_modifiedKey(i_mk)
{
}

//
Action::Type ActionKey::getType() const
{
  return Type_key;
}

// create clone
Action *ActionKey::clone() const
{
  return new ActionKey(m_modifiedKey);
}

// stream output
tostream &ActionKey::output(tostream &i_ost) const
{
  return i_ost << m_modifiedKey;
}

//
ActionKeySeq::ActionKeySeq(KeySeq *i_keySeq)
  : m_keySeq(i_keySeq)
{
}

//
Action::Type ActionKeySeq::getType() const
{
  return Type_keySeq;
}

// create clone
Action *ActionKeySeq::clone() const
{
  return new ActionKeySeq(m_keySeq);
}

// stream output
tostream &ActionKeySeq::output(tostream &i_ost) const
{
  return i_ost << _T("$") << m_keySeq->getName();
}

//
ActionFunction::ActionFunction(FunctionData *i_functionData,
			       Modifier i_modifier)
  : m_functionData(i_functionData),
    m_modifier(i_modifier)
{
}

//
ActionFunction::~ActionFunction()
{
  delete m_functionData;
}

//
Action::Type ActionFunction::getType() const
{
  return Type_function;
}

// create clone
Action *ActionFunction::clone() const
{
  return new ActionFunction(m_functionData->clone(), m_modifier);
}

// stream output
tostream &ActionFunction::output(tostream &i_ost) const
{
  return i_ost << m_modifier << m_functionData;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// KeySeq


void KeySeq::copy()
{
  for (Actions::iterator i = m_actions.begin(); i != m_actions.end(); ++ i)
    (*i) = (*i)->clone();
}


void KeySeq::clear()
{
  for (Actions::iterator i = m_actions.begin(); i != m_actions.end(); ++ i)
    delete (*i);
}


KeySeq::KeySeq(const tstringi &i_name)
  : m_name(i_name)
{
}


KeySeq::KeySeq(const KeySeq &i_ks)
  : m_actions(i_ks.m_actions),
    m_name(i_ks.m_name)
{
  copy();
}


KeySeq::~KeySeq()
{
  clear();
}


KeySeq &KeySeq::operator=(const KeySeq &i_ks)
{
  if (this != &i_ks)
  {
    clear();
    m_actions = i_ks.m_actions;
    copy();
  }
  return *this;
}


KeySeq &KeySeq::add(const Action &i_action)
{
  m_actions.push_back(i_action.clone());
  return *this;
}


/// get the first modified key of this key sequence
ModifiedKey KeySeq::getFirstModifiedKey() const
{
  if (0 < m_actions.size())
  {
    const Action *a = m_actions.front();
    switch (a->getType())
    {
      case Action::Type_key:
	return reinterpret_cast<const ActionKey *>(a)->m_modifiedKey;
      case Action::Type_keySeq:
	return reinterpret_cast<const ActionKeySeq *>(a)->
	  m_keySeq->getFirstModifiedKey();
      default:
	break;
    }
  }
  return ModifiedKey();				// failed
}


// stream output
tostream &operator<<(tostream &i_ost, const KeySeq &i_ks)
{
  for (KeySeq::Actions::const_iterator
	 i = i_ks.m_actions.begin(); i != i_ks.m_actions.end(); ++ i)
    i_ost << **i << _T(" ");
  return i_ost;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Keymap


Keymap::KeyAssignments &Keymap::getKeyAssignments(const ModifiedKey &i_mk)
{
  ASSERT(1 <= i_mk.m_key->getScanCodesSize());
  return m_hashedKeyAssignments[i_mk.m_key->getScanCodes()->m_scan %
			       HASHED_KEY_ASSIGNMENT_SIZE];
}

const Keymap::KeyAssignments &
Keymap::getKeyAssignments(const ModifiedKey &i_mk) const
{
  ASSERT(1 <= i_mk.m_key->getScanCodesSize());
  return m_hashedKeyAssignments[i_mk.m_key->getScanCodes()->m_scan %
			       HASHED_KEY_ASSIGNMENT_SIZE];
}


Keymap::Keymap(Type i_type,
	       const tstringi &i_name,
	       const tstringi &i_windowClass,
	       const tstringi &i_windowTitle,
	       KeySeq *i_defaultKeySeq,
	       Keymap *i_parentKeymap)
  : m_type(i_type),
    m_name(i_name),
    m_defaultKeySeq(i_defaultKeySeq),
    m_parentKeymap(i_parentKeymap),
    m_windowClass(_T(".*")),
    m_windowTitle(_T(".*"))
{
  if (i_type == Type_windowAnd || i_type == Type_windowOr)
    try
    {
      boost::regbase::flag_type f = (boost::regbase::normal | 
				     boost::regbase::icase |
				     boost::regbase::use_except);
      if (!i_windowClass.empty())
	m_windowClass.assign(i_windowClass, f);
      if (!i_windowTitle.empty())
	m_windowTitle.assign(i_windowTitle, f);
    }
    catch (boost::bad_expression &i_e)
    {
      throw ErrorMessage() << i_e.what();
    }
}


// add a key assignment;
void Keymap::addAssignment(const ModifiedKey &i_mk, KeySeq *i_keySeq)
{
  KeyAssignments &ka = getKeyAssignments(i_mk);
  for (KeyAssignments::iterator i = ka.begin(); i != ka.end(); ++ i)
    if ((*i).m_modifiedKey == i_mk)
    {
      (*i).m_keySeq = i_keySeq;
      return;
    }
  ka.push_front(KeyAssignment(i_mk, i_keySeq));
}


// add modifier
void Keymap::addModifier(Modifier::Type i_mt, AssignOperator i_ao,
			 AssignMode i_am, Key *i_key)
{
  if (i_ao == AO_new)
    m_modAssignments[i_mt].clear();
  else
  {
    for (ModAssignments::iterator i = m_modAssignments[i_mt].begin();
	 i != m_modAssignments[i_mt].end(); ++ i)
      if ((*i).m_key == i_key)
      {
	(*i).m_assignOperator = i_ao;
	(*i).m_assignMode = i_am;
	return;
      }
  }
  ModAssignment ma;
  ma.m_assignOperator = i_ao;
  ma.m_assignMode = i_am;
  ma.m_key = i_key;
  m_modAssignments[i_mt].push_back(ma);
}

  
// search
const Keymap::KeyAssignment *
Keymap::searchAssignment(const ModifiedKey &i_mk) const
{
  const KeyAssignments &ka = getKeyAssignments(i_mk);
  for (KeyAssignments::const_iterator i = ka.begin(); i != ka.end(); ++ i)
    if ((*i).m_modifiedKey.m_key == i_mk.m_key &&
	(*i).m_modifiedKey.m_modifier.doesMatch(i_mk.m_modifier))
      return &(*i);
  return NULL;
}


// does same window
bool Keymap::doesSameWindow(const tstringi i_className,
			    const tstringi &i_titleName)
{
  if (m_type == Type_keymap)
    return false;

  if (boost::regex_search(i_className, tcmatch_results(), m_windowClass))
  {
    if (m_type == Type_windowAnd)
      return boost::regex_search(i_titleName,
				 tcmatch_results(), m_windowTitle);
    else // type == Type_windowOr
      return true;
  }
  else
  {
    if (m_type == Type_windowAnd)
      return false;
    else // type == Type_windowOr
      return boost::regex_search(i_titleName, tcmatch_results(),
				 m_windowTitle);
  }
}


// adjust modifier
void Keymap::adjustModifier(Keyboard &i_keyboard)
{
  for (int i = 0; i < Modifier::Type_end; ++ i)
  {
    ModAssignments mos;
    if (m_parentKeymap)
      mos = m_parentKeymap->m_modAssignments[i];
    else
    {
      // set default modifiers
      if (i < Modifier::Type_BASIC)
      {
	Keyboard::Mods mods =
	  i_keyboard.getModifiers(static_cast<Modifier::Type>(i));
	for (Keyboard::Mods::iterator j = mods.begin(); j != mods.end(); ++ j)
	{
	  ModAssignment ma;
	  ma.m_assignOperator = AO_add;
	  ma.m_assignMode = AM_normal;
	  ma.m_key = *j;
	  mos.push_back(ma);
	}
      }
    }
    
    // mod adjust
    for (ModAssignments::iterator mai = m_modAssignments[i].begin();
	 mai != m_modAssignments[i].end(); ++ mai)
    {
      ModAssignment ma = *mai;
      ma.m_assignOperator = AO_new;
      switch ((*mai).m_assignOperator)
      {
	case AO_new:
	{
	  mos.clear();
	  mos.push_back(ma);
	  break;
	}
	case AO_add:
	{
	  mos.push_back(ma);
	  break;
	}
	case AO_sub:
	{
	  for (ModAssignments::iterator j = mos.begin();
	       j != mos.end(); ++ j)
	    if ((*j).m_key == ma.m_key)
	    {
	      mos.erase(j);
	      goto break_for;
	    }
	  break;
	}
	case AO_overwrite:
	{
	  for (ModAssignments::iterator j = mos.begin();
	       j != mos.end(); ++ j)
	    (*j).m_assignMode = (*mai).m_assignMode;
	  break;
	}
      }
    }
    break_for:

    // erase redundant modifiers
    for (ModAssignments::iterator j = mos.begin(); j != mos.end(); ++ j)
    {
      ModAssignments::iterator k;
      for (k = j, ++ k; k != mos.end(); ++ k)
	if ((*k).m_key == (*j).m_key)
	  break;
      if (k != mos.end())
      {
	k = j;
	++ j;
	mos.erase(k);
	break;
      }
    }
    
    m_modAssignments[i] = mos;
  }
}

// describe
void Keymap::describe(tostream &i_ost, DescribedKeys *o_dk) const
{
  switch (m_type)
  {
    case Type_keymap:
      i_ost << _T("keymap ") << m_name;
      break;
    case Type_windowAnd:
      i_ost << _T("window ") << m_name << _T(" ");
      if (m_windowTitle.str() == _T(".*"))
	i_ost << _T("/") << m_windowClass.str() << _T("/");
      else
	i_ost << _T("( /") << m_windowClass.str() << _T("/ && /")
	      << m_windowTitle.str() << _T("/ )");
      break;
    case Type_windowOr:
      i_ost << _T("window ") << m_name << _T(" ( /")
	    << m_windowClass.str() << _T("/ || /") << m_windowTitle.str()
	    << _T("/ )");
      break;
  }
  if (m_parentKeymap)
    i_ost << _T(" : ") << m_parentKeymap->m_name;
  i_ost << _T(" => ") << *m_defaultKeySeq << std::endl;

  typedef std::vector<KeyAssignment> SortedKeyAssignments;
  SortedKeyAssignments ska;
  for (size_t i = 0; i < HASHED_KEY_ASSIGNMENT_SIZE; ++ i)
  {
    const KeyAssignments &ka = m_hashedKeyAssignments[i];
    for (KeyAssignments::const_iterator j = ka.begin(); j != ka.end(); ++ j)
      ska.push_back(*j);
  }
  std::sort(ska.begin(), ska.end());
  for (SortedKeyAssignments::iterator i = ska.begin(); i != ska.end(); ++ i)
  {
    if (o_dk)
    {
      DescribedKeys::iterator
	j = std::find(o_dk->begin(), o_dk->end(), i->m_modifiedKey);
      if (j != o_dk->end())
	continue;
    }
    i_ost << _T(" key ") << i->m_modifiedKey << _T("\t=> ")
	  << *i->m_keySeq << std::endl;
    if (o_dk)
      o_dk->push_back(i->m_modifiedKey);
  }
  i_ost << std::endl;

  if (m_parentKeymap)
    m_parentKeymap->describe(i_ost, o_dk);
}

// set default keySeq and parent keymap if default keySeq has not been set
bool Keymap::setIfNotYet(KeySeq *i_keySeq, Keymap *i_parentKeymap)
{
  if (m_defaultKeySeq)
    return false;
  m_defaultKeySeq = i_keySeq;
  m_parentKeymap = i_parentKeymap;
  return true;
}

// stream output
extern tostream &operator<<(tostream &i_ost, const Keymap *i_keymap)
{
  return i_ost << i_keymap->getName();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Keymaps


Keymaps::Keymaps()
{
}


// search by name
Keymap *Keymaps::searchByName(const tstringi &i_name)
{
  for (KeymapList::iterator
	 i = m_keymapList.begin(); i != m_keymapList.end(); ++ i)
    if ((*i).getName() == i_name)
      return &*i;
  return NULL;
}


// search window
void Keymaps::searchWindow(KeymapPtrList *o_keymapPtrList,
			   const tstringi &i_className,
			   const tstringi &i_titleName)
{
  o_keymapPtrList->clear();
  for (KeymapList::iterator
	 i = m_keymapList.begin(); i != m_keymapList.end(); ++ i)
    if ((*i).doesSameWindow(i_className, i_titleName))
      o_keymapPtrList->push_back(&(*i));
}


// add keymap
Keymap *Keymaps::add(const Keymap &i_keymap)
{
  if (Keymap *k = searchByName(i_keymap.getName()))
    return k;
  m_keymapList.push_front(i_keymap);
  return &m_keymapList.front();
}


// adjust modifier
void Keymaps::adjustModifier(Keyboard &i_keyboard)
{
  for (KeymapList::reverse_iterator i = m_keymapList.rbegin();
       i != m_keymapList.rend(); ++ i)
    (*i).adjustModifier(i_keyboard);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// KeySeqs


// add a named keyseq (name can be empty)
KeySeq *KeySeqs::add(const KeySeq &i_keySeq)
{
  if (!i_keySeq.getName().empty())
  {
    KeySeq *ks = searchByName(i_keySeq.getName());
    if (ks)
      return &(*ks = i_keySeq);
  }
  m_keySeqList.push_front(i_keySeq);
  return &m_keySeqList.front();
}


// search by name
KeySeq *KeySeqs::searchByName(const tstringi &i_name)
{
  for (KeySeqList::iterator
	 i = m_keySeqList.begin(); i != m_keySeqList.end(); ++ i)
    if ((*i).getName() == i_name)
      return &(*i);
  return NULL;
}
