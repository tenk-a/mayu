///////////////////////////////////////////////////////////////////////////////
// keyboard.h


#ifndef __keyboard_h__
#define __keyboard_h__


#include "misc.h"

#include "driver.h"
#include "stringtool.h"

#include <vector>
#include <list>
#include <map>


using StringTool::istring;


class ScanCode
{
public:
  enum
  {
    BREAK = KEYBOARD_INPUT_DATA::BREAK,	// BREAK is key release
    E0    = KEYBOARD_INPUT_DATA::E0,	// E0, E1 are for extended key,
    E1    = KEYBOARD_INPUT_DATA::E1,
    E0E1  = KEYBOARD_INPUT_DATA::E0E1
  };
  u_char scan;
  u_char flags;

public:
  ScanCode() : scan(0), flags(0) { }
  ScanCode(u_char scan_, u_char flags_) : scan(scan_), flags(flags_) { }
  
  bool operator==(const ScanCode &sc) const
  { return (scan == sc.scan && (E0E1 & flags) == (E0E1 & sc.flags)); }
  bool operator!=(const ScanCode &sc) const { return !(*this == sc); }
};


class Key
{
public:
  enum { maxlengthof_scanCodes = 4 };

  bool isPressed;			// if this key pressed physically
  bool isPressedOnWin32;		// if this key pressed on Win32
  bool isPressedByAssign;		// if this key pressed by assign

private:
  std::vector<istring> names;	// key name
  
  // key scan code
  size_t lengthof_scanCodes;
  ScanCode scanCodes[maxlengthof_scanCodes];

public:
  Key()
    : isPressed(false),
      isPressedOnWin32(false),
      isPressedByAssign(false),
      lengthof_scanCodes(0)
  { }

  // get key name (first name)
  const istring &getName() const { return names.at(0); }

  // get scan codes
  const ScanCode *getScanCodes() const { return scanCodes; }
  size_t getLengthof_scanCodes() const { return lengthof_scanCodes; }
  
  // add a name of key
  void addName(const istring &name);
  
  // add a scan code
  void addScanCode(const ScanCode &sc);
  
  // initializer
  Key &initialize();
  
  // equation by name
  bool operator==(const istring &name) const;
  bool operator!=(const istring &name) const { return !(*this == name); }
  
  // is the scan code of this key ?
  bool isSameScanCode(const Key &key) const;
  
  // is the key's scan code the prefix of this key's scan code ?
  bool isPrefixScanCode(const Key &key) const;
};


class Modifier
{
  typedef u_long MODIFIERS;
  MODIFIERS modifiers;
  MODIFIERS dontcares;
  
public:
  enum Type
  {
    begin = 0,
    Shift = begin, Alt, Control, Windows, BASIC,	// <BASIC_MODIFIER>
    Up = BASIC, Down, KEYSEQ,				// <KEYSEQ_MODIFIER>
    ImeLock = KEYSEQ, ImeComp,				// <ASSIGN_MODIFIER>
    NumLock, CapsLock, ScrollLock,
    Mod0, Mod1, Mod2, Mod3, Mod4, Mod5, Mod6, Mod7, Mod8, Mod9,
    Lock0, Lock1, Lock2, Lock3, Lock4, Lock5, Lock6, Lock7, Lock8, Lock9,
    ASSIGN, end = ASSIGN
  };
  
public:
  Modifier();
  Modifier &on      (Type t) { return press(t); }
  Modifier &off     (Type t) { return release(t); }
  Modifier &press   (Type t) {modifiers|= ((MODIFIERS(1))<<t); return care(t);}
  Modifier &release (Type t) {modifiers&=~((MODIFIERS(1))<<t); return care(t);}
  Modifier &care    (Type t) {dontcares&=~((MODIFIERS(1))<<t); return *this;}
  Modifier &dontcare(Type t) {dontcares|= ((MODIFIERS(1))<<t); return *this;}

  Modifier &on      (Type t, bool isOn_) { return press(t, isOn_); }
  Modifier &press   (Type t, bool isPressed_)
  { return isPressed_ ? press(t) : release(t); }
  Modifier &care    (Type t, bool doCare)
  { return doCare ? care(t) : dontcare(t); }
  
  bool operator==(const Modifier &m) const
  { return modifiers == m.modifiers && dontcares == m.dontcares; }

  // add m's modifiers where this dontcare
  Modifier &operator+=(const Modifier &m);

  // does match (except dontcare modifiers)
  // (is the m included *this set ?)
  bool doesMatch(const Modifier &m) const
  { return ((modifiers | dontcares) == (m.modifiers | dontcares)); }
  
  bool isOn(Type t) const { return isPressed(t); }
  bool isPressed(Type t) const { return !!(modifiers & ((MODIFIERS(1))<<t)); }
  bool isDontcare(Type t) const { return !!(dontcares & ((MODIFIERS(1))<<t)); }
};


class ModifiedKey
{
public:
  Modifier modifier;
  Key *key;
  
public:
  ModifiedKey() : key(NULL) { }
  ModifiedKey(Key *key_) : key(key_) { }
  ModifiedKey(const Modifier &m, Key *key_)  : modifier(m), key(key_) { }
  bool operator==(const ModifiedKey &mk) const
  { return modifier == mk.modifier && key == mk.key; }
};


class Keyboard
{
  // keyboard keys (hashed by first scan code)
  // Keys must be *list* of Key, because
  //   *pointers* into Keys exist anywhere.
  enum { lengthof_hashedKeys = 128 };
  typedef std::list<Key> Keys;
  Keys hashedKeys[lengthof_hashedKeys];

  // key name aliases
  typedef std::map<istring, Key *> Aliases;
  Aliases aliases;
  
  // key used to synchronize
  Key syncKey;

  // keyboard modifiers (pointer into Keys)
public:
  typedef std::list<Key *> Mods;
private:
  Mods mods[Modifier::BASIC];

public:
  class KeyIterator
  {
    Keys *hashedKeys;
    size_t lengthof_hashedKeys;
    Keys::iterator i;
    
    void next();
    
  public:
    KeyIterator(Keys *hashedKeys_, size_t lengthof_hashedKeys_);
    Key *operator *();
    void operator++(int) { next(); }
  };
  
private:
  Keys &getKeys(const Key &key);

public:
  // add a key
  void addKey(const Key &key);

  // add a key name alias
  void addAlias(const istring &aliasName, Key *key);
  
  // get a sync key
  Key *getSyncKey() { return &syncKey; }
  
  // add a modifier key
  void addModifier(Modifier::Type mt, Key * key);
  
  // search a key
  Key *searchKey(const Key &key);
  
  // search a key (of which the key's scan code is the prefix)
  Key *searchPrefixKey(const Key &key);
  
  // search a key by name
  Key *searchKey(const istring &name);

  // search a key by non-alias name
  Key *searchKeyByNonAliasName(const istring &name);

  // get modifiers
  Mods &getModifiers(Modifier::Type mt) { return mods[mt]; }

  // get key iterator
  KeyIterator getKeyIterator()
  { return KeyIterator(&hashedKeys[0], lengthof_hashedKeys); }
};


#endif // __keyboard_h__
