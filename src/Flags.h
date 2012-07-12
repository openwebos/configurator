// @@@LICENSE
//
//      Copyright (c) 2009-2012 Hewlett-Packard Development Company, L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// LICENSE@@@

#ifndef FLAGS_H
#define FLAGS_H

template <class Enum>
class Flags {
public:
	typedef Enum enum_type;

	Flags(const Flags& other)
		: m_flags(other.m_flags)
	{
	}

	Flags(Enum flag)
		: m_flags(flag)
	{
	}

	Flags()
		: m_flags(0)
	{
	}

	~Flags()
	{
	}

	operator int() const
	{
		return (int)m_flags;
	}

	bool operator!() const
	{
		return !m_flags;
	}

	Flags& operator=(const Flags& other)
	{
		m_flags = other.m_flags;
		return *this;
	}

	Flags operator&(Enum mask) const
	{
		return Flags(m_flags & mask);
	}

	Flags operator&(Flags mask) const
	{
		return Flags(m_flags & mask.m_flags);
	}

	Flags& operator&=(Enum mask)
	{
		m_flags &= mask;
		return *this;
	}

	Flags& operator&=(Flags mask)
	{
		m_flags &= mask.m_flags;
		return *this;
	}

	Flags operator^(Enum other) const
	{
		return Flags(m_flags ^ other);
	}

	Flags operator^(Flags other) const
	{
		return Flags(m_flags ^ other);
	}

	Flags& operator^=(Enum other)
	{
		m_flags ^= other;
		return *this;
	}

	Flags& operator^=(const Flags& other)
	{
		m_flags ^= other.m_flags;
		return *this;
	}

	Flags operator|(Enum other) const
	{
		return Flags(m_flags | other);
	}

	Flags operator|(Flags other) const
	{
		return Flags(m_flags | other.m_flags);
	}

	Flags& operator|=(Enum other)
	{
		m_flags |= other;
		return *this;
	}

	Flags& operator|=(Flags other)
	{
		m_flags |= other.m_flags;
		return *this;
	}

private:
	Flags(unsigned int flags)
		: m_flags(flags)
	{
	}

	unsigned int m_flags;
};

#define DECLARE_FLAGS(Name, Enum) typedef Flags<Enum> Name

#define DECLARE_OPERATORS_FOR_FLAGS(Name) \
inline Flags<Name::enum_type> operator|(Name::enum_type e1, Name::enum_type e2) \
{ return Flags<Name::enum_type>(e1) | e2; }\
inline Flags<Name::enum_type> operator|(Name::enum_type e1, Flags<Name::enum_type> f2) \
{ return f2 | e1; } \
inline Name::enum_type operator~(Name::enum_type e1) \
{ return ((Name::enum_type)~e1); }

#endif // FLAGS_H
