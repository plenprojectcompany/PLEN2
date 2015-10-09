/*!
	@file      Purser.h
	@brief     各種パーサクラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _UTILITY__PURSER_H_
#define _UTILITY__PURSER_H_

namespace Utility
{
	class AbstractPurser;
	class NilPurser;
	class CharGroupPurser;
	class StringGroupPurser;
	class HexStringPurser;
}


/*!
	@brief 抽象パーサクラス(インタフェース)
*/
class Utility::AbstractPurser
{
protected:
	char m_index;

	AbstractPurser();

public:
	virtual ~AbstractPurser();

	virtual bool purse(const char* input) = 0;
	virtual const char& index();
};


/*!
	@brief 常に受理するパーサクラス
*/
class Utility::NilPurser : public Utility::AbstractPurser
{
public:
	NilPurser();
	virtual ~NilPurser();

	virtual bool purse(const char* input);
};


/*!
	@brief 与えられた文字グループのみ受理するパーサクラス

	@note
	CharGroupPurserは内部でリニアサーチを行うため、
	受理優先度の高い文字ほど先頭に配置すると効果的です。
*/
class Utility::CharGroupPurser : public Utility::AbstractPurser
{
private:
	const char* m_accept_chars;

public:
	CharGroupPurser(const char* accept_chars);
	virtual ~CharGroupPurser();

	/*!
		@brief パースを行うメソッド

		@note
		現在の実装では、与えられた文字列の先頭に対してのみパースを行います。
		すなわち、/^[accept_chars]/であるかを検証します。
		<br><br>
		/^[accept_chars]+$/であるかを検証する挙動に変えたい場合は、
		直接ソースコードを編集してください。
	*/
	virtual bool purse(const char* input);
};


/*!
	@brief 与えられた文字列グループのみ受理するパーサクラス

	@attention
	StringGroupPurserは内部でバイナリサーチを行うため、
	accept_strsには必ずソートした状態の配列を与えてください。
*/
class Utility::StringGroupPurser : public Utility::AbstractPurser
{
private:
	const char** m_accept_strs;
	const unsigned char m_size;

public:
	StringGroupPurser(const char* accept_strs[], const unsigned char size);
	virtual ~StringGroupPurser();
	
	virtual bool purse(const char* input);
};


/*!
	@brief 16進文字列のみ受理するパーサクラス
*/
class Utility::HexStringPurser : public Utility::AbstractPurser
{
public:
	HexStringPurser();
	virtual ~HexStringPurser();

	virtual bool purse(const char* input);
};

#endif // _UTILITY__PURSER_H_