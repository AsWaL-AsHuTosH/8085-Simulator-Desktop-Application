#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<map>

class Utility
{
public:
	static void _8Bit(std::string& data);

	static void _16Bit(std::string& data);

	static int _8bit_2sc(int num);

	static int _set_bits_count(int num);

	static void _8Bit_Normalization(int& num);//function to reset all bits after 8th bit

	static void _16Bit_Normalization(int& num);
};


void Utility::_8Bit(std::string& data)
{
	data = std::string(2 - data.length(), '0') + data;
}

void Utility::_16Bit(std::string& data)
{
	data = std::string(4 - data.length(), '0') + data;
}

int Utility::_8bit_2sc(int num)
{
	for (int i = 0; i < 8; ++i)
	{
		num ^= (1 << i);
	}

	num += 0x01;
	_8Bit_Normalization(num);
	return num;
}

int Utility::_set_bits_count(int num)
{
	int set_bits = 0;
	for (int i = 0; i < 8; ++i)
	{
		set_bits += bool(num & (1 << i));
	}

	return set_bits;
}

void Utility::_8Bit_Normalization(int& num)//function to reset all bits after 8th bit
{
	for (int i = 8; i < 32; ++i)
	{
		num &= ~(1 << i);
	}
}

void Utility::_16Bit_Normalization(int& num)
{
	for (int i = 16; i < 32; ++i)
	{
		num &= ~(1 << i);
	}
}



class Converter
{
public:
	static int HexToDec(std::string number);

	static int HexToDec(char number);

	static std::string DecToHex(int number, int type = 8);

	static void ToUpperString(std::string& s);
};

int Converter::HexToDec(std::string number)
{
	std::stringstream ss;
	ss << number;
	int decimal_number;
	ss >> std::hex >> decimal_number;
	return decimal_number;
}

int Converter::HexToDec(char number)
{
	std::stringstream ss;
	ss << number;
	int decimal_number;
	ss >> std::hex >> decimal_number;
	return decimal_number;
}


std::string Converter::DecToHex(int number, int type)
{
	std::stringstream ss;
	ss << std::hex << number;
	std::string hex_number = ss.str();
	ToUpperString(hex_number);
	if (type == 8)
	{
		Utility::_8Bit(hex_number);
	}
	else
	{
		Utility::_16Bit(hex_number);
	}
	return hex_number;
}

void Converter::ToUpperString(std::string& s)
{
	for (char& c : s)
	{
		c = toupper(c);
	}
}


class MemoryManager 
{
public:
	static std::vector<int> Memory;
	static void SetMemory(const std::string&, const std::string&);
	static void ResetMemory();
};

std::vector<int> MemoryManager::Memory(1 << 16);

void MemoryManager::SetMemory(const std::string& location, const std::string& data)
{
	Memory[Converter::HexToDec(location)] = Converter::HexToDec(data);
}

void MemoryManager::ResetMemory()
{
	Memory.clear();
	Memory.assign(1 << 16, 0);
}



class Register
{
public:
	static std::map<char, int> Main;
	static int PC;

	class Flag
	{
	public:
		static bool SF, ZF, AC, PF, CY;
	};

	static bool isValid(char reg);

	static int HL();

	static int BC();

	static int DE();

	static void UpdateFlags(int aux);

	static void Clear();
};

int Register::PC = 0;
std::map<char, int> Register::Main = { {'A', 0}, {'B', 0}, {'C' ,0}, {'D' ,0}, {'E' ,0}, {'H' ,0}, {'L' ,0} };
bool Register::Flag::AC;
bool Register::Flag::CY;
bool Register::Flag::PF;
bool Register::Flag::SF;
bool Register::Flag::ZF;

bool Register::isValid(char reg)
{
	return (reg == 'A' || reg == 'B' || reg == 'C' || reg == 'D' || reg == 'E' || reg == 'H' || reg == 'L');
}

int Register::HL()
{
	return Converter::HexToDec(Converter::DecToHex(Main['H']) + Converter::DecToHex(Main['L']));
}

int Register::BC()
{
	return Converter::HexToDec(Converter::DecToHex(Main['B']) + Converter::DecToHex(Main['C']));
}

int Register::DE()
{
	return Converter::HexToDec(Converter::DecToHex(Main['D']) + Converter::DecToHex(Main['E']));
}

void Register::UpdateFlags(int aux)
{
	/*
		Some calculation also cause carry at 10th bit but all online 8085 sim do not consider that as carry.
		Considering the above fact, I am doing the same.
		Eg: CY(1), A(80), B(40)
		and after executing the SBB B instruction the result should be:
		2sc(1H)  ------->	1111 1111
		A(80H)	 ------->	1000 0000
		2sc(40H) ------->	1100 0000
						 -------------
						 10 0011 1111
						 -------------
		Here carry is generated from 8th bit(D7) is '10' but because the set bit falls on 10th bit(D9),
		it is not considered as carry.
	*/
	Register::Flag::CY = Register::Main['A'] & (1 << 8); //@Carry Flag
	Utility::_8Bit_Normalization(Register::Main['A']);

	Register::Flag::SF = Register::Main['A'] & (1 << 7); //@Sign Flag

	Register::Flag::ZF = Register::Main['A'] == 0; //@Zero Flag

	Register::Flag::AC = aux > 0xf; //@Aux Carry Flag

	Register::Flag::PF = !(Utility::_set_bits_count(Register::Main['A']) & 1); //@Pairty Flag
}


void Register::Clear()
{
	Register::PC = 0;

	Register::Flag::AC = false;
	Register::Flag::CY = false;
	Register::Flag::PF = false;
	Register::Flag::SF = false;
	Register::Flag::ZF = false;

	for (std::pair<const char, int>& reg : Main)
	{
		reg.second = 0;
	}
}


class Instruction
{
public:
	std::string mnemonic;
	std::pair<std::string, std::string> operands;
};


class Program
{

public:
	static std::map<std::string, int> Loop;
	static std::vector<Instruction> program;

	static bool HLT;

	static bool Read(std::string filePath);

	static void Run();
};

std::vector<Instruction> Program::program;
std::map<std::string, int> Program::Loop;
bool Program::HLT;


class Error
{
public:
	static void Throw(const std::string& e, const int PC);
};

void Error::Throw(const std::string& e, const int PC = -1)
{
	std::string error;
	if (PC ^ -1)
	{
		error = "Error: " + e + " [Line Number: " + std::to_string(PC + 1) + "]";
	}
	else
	{
		error = "Error: " + e;
	}
	
	wxMessageBox(error, "Execution Stopped");
	Program::HLT = true;
}


class Mnemonic
{
public:
	static std::map<std::string, void (*)(const std::pair<std::string, std::string>&)> Execute;

	static void LoadInsctructionSet();


	static bool isValid(const std::string mnemonic);

	static void MOV(const std::pair<std::string, std::string>& operands);

	static void MVI(const std::pair<std::string, std::string>& operands);

	static void LDA(const std::pair<std::string, std::string>& operands);

	static void STA(const std::pair<std::string, std::string>& operands);

	static void LHLD(const std::pair<std::string, std::string>& operands);

	static void SHLD(const std::pair<std::string, std::string>& operands);


	static void LXI(const std::pair<std::string, std::string>& operands);

	static void LDAX(const std::pair<std::string, std::string>& operands);

	static void STAX(const std::pair<std::string, std::string>& operands);

	static void XCHG(const std::pair<std::string, std::string>& operands);

	//@Airthmatic Instrunction
	static void ADD(const std::pair<std::string, std::string>& operands);

	static void ADC(const std::pair<std::string, std::string>& operands);

	static void ADI(const std::pair<std::string, std::string>& operands);


	static void ACI(const std::pair<std::string, std::string>& operands);

	static void SUB(const std::pair<std::string, std::string>& operands);

	static void SBB(const std::pair<std::string, std::string>& operands);//Not sure about flags

	static void SUI(const std::pair<std::string, std::string>& operands);


	static void SBI(const std::pair<std::string, std::string>& operands);

	static void INR(const std::pair<std::string, std::string>& operands);//CY is not affected

	static void INX(const std::pair<std::string, std::string>& operands);//No flags are affected during the execution

	static void DCR(const std::pair<std::string, std::string>& operands);//CY is not affected


	static void DCX(const std::pair<std::string, std::string>& operands);//No flags are affected during the execution


	static void DAD(const std::pair<std::string, std::string>& operands);//only affect CY

	//@Logical Instructions

	static void ANA(const std::pair<std::string, std::string>& operands);

	static void ANI(const std::pair<std::string, std::string>& operands);

	static void ORA(const std::pair<std::string, std::string>& operands);

	static void ORI(const std::pair<std::string, std::string>& operands);

	static void XRA(const std::pair<std::string, std::string>& operands);

	static void XRI(const std::pair<std::string, std::string>& operands);

	static void CMA(const std::pair<std::string, std::string>& operands);//Flags are not affected by this instruction

	static void RLC(const std::pair<std::string, std::string>& operands);

	static void RAL(const std::pair<std::string, std::string>& operands);

	static void RRC(const std::pair<std::string, std::string>& operands);

	static void RAR(const std::pair<std::string, std::string>& operands);

	static void STC(const std::pair<std::string, std::string>& operands);

	static void CMC(const std::pair<std::string, std::string>& operands);

	static void CMP(const std::pair<std::string, std::string>& operands);

	static void CPI(const std::pair<std::string, std::string>& operands);

	//@Branching Instructions
	static void JMP(const std::pair<std::string, std::string>& operands);

	static void JC(const std::pair<std::string, std::string>& operands);

	static void JNC(const std::pair<std::string, std::string>& operands);

	static void JZ(const std::pair<std::string, std::string>& operands);

	static void JNZ(const std::pair<std::string, std::string>& operands);

	static void JPE(const std::pair<std::string, std::string>& operands);

	static void JPO(const std::pair<std::string, std::string>& operands);

	static void JM(const std::pair<std::string, std::string>& operands);

	static void JP(const std::pair<std::string, std::string>& operands);

	static void HLT(const std::pair<std::string, std::string>& operands);

	static bool validOperandCount(const std::pair<std::string, std::string>& operands, int operand_count);

};

std::map<std::string, void (*)(const std::pair<std::string, std::string>&)> Mnemonic::Execute;

bool Mnemonic::validOperandCount(const std::pair<std::string, std::string>& operands, int expected_operand_count)
{
	if (expected_operand_count == 0)
	{
		return operands.first.empty() && operands.second.empty();
	}
	else if(expected_operand_count == 1)
	{
		return !operands.first.empty() && operands.second.empty();
	}
	else
	{
		return !operands.first.empty() && !operands.second.empty();
	}
}

void Mnemonic::LoadInsctructionSet()
{
	Execute["MOV"] = MOV;
	Execute["MVI"] = MVI;
	Execute["LDA"] = LDA;
	Execute["STA"] = STA;
	Execute["LHLD"] = LHLD;
	Execute["SHLD"] = SHLD;
	Execute["HLT"] = HLT;
	Execute["LXI"] = LXI;
	Execute["LDAX"] = LDAX;
	Execute["STAX"] = STAX;
	Execute["XCHG"] = XCHG;
	Execute["ADD"] = ADD;
	Execute["ADI"] = ADI;
	Execute["ADC"] = ADC;
	Execute["ACI"] = ACI;
	Execute["SUB"] = SUB;
	Execute["SBB"] = SBB;
	Execute["SUI"] = SUI;
	Execute["SBI"] = SBI;
	Execute["INR"] = INR;
	Execute["INX"] = INX;
	Execute["DCR"] = DCR;
	Execute["DCX"] = DCX;
	Execute["DAD"] = DAD;
	Execute["ANA"] = ANA;
	Execute["ANI"] = ANI;
	Execute["ORA"] = ORA;
	Execute["ORI"] = ORI;
	Execute["XRA"] = XRA;
	Execute["XRI"] = XRI;
	Execute["CMA"] = CMA;
	Execute["RLC"] = RLC;
	Execute["RAL"] = RAL;
	Execute["RRC"] = RRC;
	Execute["RAR"] = RAR;
	Execute["STC"] = STC;
	Execute["CMC"] = CMC;
	Execute["CMP"] = CMP;
	Execute["CPI"] = CPI;
	Execute["JMP"] = JMP;
	Execute["JC"] = JC;
	Execute["JNC"] = JNC;
	Execute["JZ"] = JZ;
	Execute["JNZ"] = JNZ;
	Execute["JPE"] = JPE;
	Execute["JPO"] = JPO;
	Execute["JM"] = JM;
	Execute["JP"] = JP;
}

bool Mnemonic::isValid(const std::string mnemonic)
{
	return Execute.count(mnemonic);
}

void Mnemonic::MOV(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 2))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}


	char destination = operands.first.front(), source = operands.second.front();
	if (destination == 'M' && Register::isValid(source))
	{
		MemoryManager::Memory[Register::HL()] = Register::Main[source];
	}
	else if (source == 'M' && Register::isValid(destination))
	{
		Register::Main[destination] = MemoryManager::Memory[Register::HL()];
	}
	else if (Register::isValid(destination) && Register::isValid(source))
	{
		Register::Main[destination] = Register::Main[source];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::MVI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 2))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}


	char reg = operands.first.front();
	std::string value = operands.second;

	int nValue = Converter::HexToDec(value);

	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else if (reg == 'M') //@Memory
	{
		MemoryManager::Memory[Register::HL()] = nValue;
	}
	else if (Register::isValid(reg))
	{
		Register::Main[reg] = nValue;
	}
	else
	{
		Error::Throw("Invalid Register.", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::LDA(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::string address = operands.first;
	int nAddress = Converter::HexToDec(address);
	if (nAddress >= 0 && nAddress < (1 << 16))
	{
		Register::Main['A'] = MemoryManager::Memory[nAddress];
	}
	else
	{
		Error::Throw("Invalid Memory Location", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::STA(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::string address = operands.first;
	int nAddress = Converter::HexToDec(address);
	if (nAddress >= 0 && nAddress < (1 << 16))
	{
		MemoryManager::Memory[nAddress] = Register::Main['A'];
	}
	else
	{
		Error::Throw("Invalid Memory Location", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::LHLD(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::string address = operands.first;
	int nAddress = Converter::HexToDec(address);
	if (nAddress >= 0 && nAddress < (1 << 16) - 1)
	{
		Register::Main['L'] = MemoryManager::Memory[nAddress];
		Register::Main['H'] = MemoryManager::Memory[nAddress + 1];
	}
	else
	{
		Error::Throw("Invalid Memory Location", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::SHLD(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::string address = operands.first;
	int nAddress = Converter::HexToDec(address);
	if (nAddress >= 0 && nAddress < (1 << 16) - 1)
	{
		MemoryManager::Memory[nAddress] = Register::Main['L'];
		MemoryManager::Memory[nAddress + 1] = Register::Main['H'];
	}
	else
	{
		Error::Throw("Invalid Memory Location", Register::PC);
	}
	++Register::PC;
}



void Mnemonic::LXI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 2))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	std::string data = operands.second;
	Utility::_16Bit(data);
	if (reg == 'B')
	{
		Register::Main['B'] = Converter::HexToDec(data.substr(0, 2));
		Register::Main['C'] = Converter::HexToDec(data.substr(2, 2));
	}
	else if (reg == 'H')
	{
		Register::Main['H'] = Converter::HexToDec(data.substr(0, 2));
		Register::Main['L'] = Converter::HexToDec(data.substr(2, 2));
	}
	else if (reg == 'D')
	{
		Register::Main['D'] = Converter::HexToDec(data.substr(0, 2));
		Register::Main['E'] = Converter::HexToDec(data.substr(2, 2));
	}
	else
	{
		Error::Throw("Invalid Register Pair", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::LDAX(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	if (reg == 'B')
	{
		Register::Main['A'] = MemoryManager::Memory[Register::BC()];
	}
	else if (reg == 'H')
	{
		Register::Main['A'] = MemoryManager::Memory[Register::HL()];
	}
	else if (reg == 'D')
	{
		Register::Main['A'] = MemoryManager::Memory[Register::DE()];
	}
	else
	{
		Error::Throw("Invalid Register Pair", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::STAX(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	if (reg == 'B')
	{
		MemoryManager::Memory[Register::BC()] = Register::Main['A'];
	}
	else if (reg == 'H')
	{
		MemoryManager::Memory[Register::HL()] = Register::Main['A'];
	}
	else if (reg == 'D')
	{
		MemoryManager::Memory[Register::DE()] = Register::Main['A'];
	}
	else
	{
		Error::Throw("Invalid Register Pair", Register::PC);
	}
	++Register::PC;
}

void Mnemonic::XCHG(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::swap(Register::Main['H'], Register::Main['D']);
	std::swap(Register::Main['L'], Register::Main['E']);
	++Register::PC;
}

//@Airthmatic Instrunction
void Mnemonic::ADD(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	int _4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back()), _4Bit_R = 0;

	if (reg == 'M')
	{
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(MemoryManager::Memory[Register::HL()])).back());
		Register::Main['A'] += MemoryManager::Memory[Register::HL()];

	}
	else if (Register::isValid(reg))
	{
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(Register::Main[reg])).back());
		Register::Main['A'] += Register::Main[reg];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}

void Mnemonic::ADC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	int _4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back()), _4Bit_R = 0;

	if (reg == 'M')
	{
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(MemoryManager::Memory[Register::HL()])).back());
		Register::Main['A'] += MemoryManager::Memory[Register::HL()] + Register::Flag::CY;

	}
	else if (Register::isValid(reg))
	{
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(Register::Main[reg])).back());
		Register::Main['A'] += Register::Main[reg] + Register::Flag::CY;
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}

void Mnemonic::ADI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first);
	int _4Bit_A = 0, _4Bit_R = 0;
	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		std::string data = operands.first;
		Utility::_8Bit(data);
		_4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back());
		_4Bit_R = Converter::HexToDec(data.back());

		Register::Main['A'] += nValue;
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}


void Mnemonic::ACI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	std::string data = operands.first;
	Utility::_8Bit(data);
	int nValue = Converter::HexToDec(data);
	int _4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back());
	int _4Bit_R = Converter::HexToDec(data.back());

	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		Register::Main['A'] += nValue + Register::Flag::CY;
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}

void Mnemonic::SUB(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	int _4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back()), _4Bit_R = 0;

	if (reg == 'M')
	{
		//@Setting carry flag explicitly
		int minuend = Register::Main['A'];
		int subtrahend = MemoryManager::Memory[Register::HL()];
		Register::Flag::CY = subtrahend > minuend;

		int _2sc = Utility::_8bit_2sc(MemoryManager::Memory[Register::HL()]);
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		Register::Main['A'] += _2sc;
	}
	else if (Register::isValid(reg))
	{
		//@Setting carry flag explicitly
		int minuend = Register::Main['A'];
		int subtrahend = Register::Main[reg];
		Register::Flag::CY = subtrahend > minuend;

		int _2sc = Utility::_8bit_2sc(Register::Main[reg]);
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		Register::Main['A'] += _2sc;
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}

void Mnemonic::SBB(const std::pair<std::string, std::string>& operands)//Not sure about flags
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	int _4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back()), _4Bit_R = 0;
	//int _4Bit_C = Register::Flag::CY ? 0xF : 0x0; // Not sure if last nibble of Carry is also responsible for AC
	if (reg == 'M')
	{

		int _2sc = Utility::_8bit_2sc(MemoryManager::Memory[Register::HL()]);
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		Register::Main['A'] += _2sc + Utility::_8bit_2sc(Register::Flag::CY);

	}
	else if (Register::isValid(reg))
	{

		int _2sc = Utility::_8bit_2sc(Register::Main[reg]);
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		Register::Main['A'] += _2sc + Utility::_8bit_2sc(Register::Flag::CY);

	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R); //Add _4Bit_C if last nibble of Carry is also responsible for AC
	++Register::PC;
}

void Mnemonic::SUI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first), _4Bit_A = 0, _4Bit_R = 0;
	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		int _2sc = Utility::_8bit_2sc(nValue);
		_4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back());
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		Register::Main['A'] += _2sc;
	}

	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}


void Mnemonic::SBI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first), _4Bit_A = 0, _4Bit_R = 0;/*, _4Bit_C = 0*/

	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		int _2sc = Utility::_8bit_2sc(nValue);
		_4Bit_A = Converter::HexToDec((Converter::DecToHex(Register::Main['A'])).back());
		_4Bit_R = Converter::HexToDec((Converter::DecToHex(_2sc)).back());
		//_4Bit_C = Register::Flag::CY ? 0xF : 0x0;// Not sure if last nibble of Carry is also responsible for AC
		Register::Main['A'] += _2sc + Utility::_8bit_2sc(Register::Flag::CY);//Add _4Bit_C if last nibble of Carry is also responsible for AC
	}
	Register::UpdateFlags(_4Bit_A + _4Bit_R);
	++Register::PC;
}

void Mnemonic::INR(const std::pair<std::string, std::string>& operands)//CY is not affected
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	if (reg == 'M')
	{
		int nValue = MemoryManager::Memory[Register::HL()];
		Register::Flag::AC = (Converter::DecToHex(nValue)).back() == 'F';//@Auxiliary carry
		++MemoryManager::Memory[Register::HL()];
		Utility::_8Bit_Normalization(MemoryManager::Memory[Register::HL()]);
		Register::Flag::PF = !(Utility::_set_bits_count(MemoryManager::Memory[Register::HL()]) & 1);//@Parity Flag
		Register::Flag::SF = MemoryManager::Memory[Register::HL()] & (1 << 7);//Sign Flag
		Register::Flag::ZF = MemoryManager::Memory[Register::HL()] == 0;//Zero Flag
	}
	else if (Register::isValid(reg))
	{
		int nValue = Register::Main[reg];
		Register::Flag::AC = (Converter::DecToHex(nValue)).back() == 'F';//@Auxiliary carry
		++Register::Main[reg];
		Utility::_8Bit_Normalization(Register::Main[reg]);
		Register::Flag::PF = !(Utility::_set_bits_count(Register::Main[reg]) & 1);//@Parity Flag
		Register::Flag::SF = Register::Main[reg] & (1 << 7);//Sign Flag
		Register::Flag::ZF = Register::Main[reg] == 0;//Zero Flag
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	++Register::PC;
}

void Mnemonic::INX(const std::pair<std::string, std::string>& operands)//No flags are affected during the execution
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	if (reg == 'H')
	{
		int DATA = Register::HL();
		++DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['H'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['L'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else if (reg == 'D')
	{
		int DATA = Register::DE();
		++DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['D'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['E'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else if (reg == 'B')
	{
		int DATA = Register::BC();
		++DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['B'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['C'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	++Register::PC;
}

void Mnemonic::DCR(const std::pair<std::string, std::string>& operands)//CY is not affected
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	if (reg == 'M')
	{
		int nValue = MemoryManager::Memory[Register::HL()];
		//Register::Flag::AC = (Converter::DecToHex(nValue)).back() == 'F';//@Not sure about auxiliary flag
		--MemoryManager::Memory[Register::HL()];
		Utility::_8Bit_Normalization(MemoryManager::Memory[Register::HL()]);
		Register::Flag::PF = !(Utility::_set_bits_count(MemoryManager::Memory[Register::HL()]) & 1);//@Parity Flag
		Register::Flag::SF = MemoryManager::Memory[Register::HL()] & (1 << 7);//Sign Flag
		Register::Flag::ZF = MemoryManager::Memory[Register::HL()] == 0;//Zero Flag
	}
	else if (Register::isValid(reg))
	{
		int nValue = Register::Main[reg];
		//Register::Flag::AC = (Converter::DecToHex(nValue)).back() == 'F';//@Not sure about auxiliary flag
		--Register::Main[reg];
		Utility::_8Bit_Normalization(Register::Main[reg]);
		Register::Flag::PF = !(Utility::_set_bits_count(Register::Main[reg]) & 1);//@Parity Flag
		Register::Flag::SF = Register::Main[reg] & (1 << 7);//Sign Flag
		Register::Flag::ZF = Register::Main[reg] == 0;//Zero Flag
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	++Register::PC;
}

void Mnemonic::DCX(const std::pair<std::string, std::string>& operands)//No flags are affected during the execution
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();

	if (reg == 'H')
	{
		int DATA = Register::HL();
		--DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['H'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['L'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else if (reg == 'D')
	{
		int DATA = Register::DE();
		--DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['D'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['E'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else if (reg == 'B')
	{
		int DATA = Register::BC();
		--DATA;
		Utility::_16Bit_Normalization(DATA);
		std::string xDATA = Converter::DecToHex(DATA, 16);
		Register::Main['B'] = Converter::HexToDec(xDATA.substr(0, 2));
		Register::Main['C'] = Converter::HexToDec(xDATA.substr(2, 2));
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	++Register::PC;
}

void Mnemonic::DAD(const std::pair<std::string, std::string>& operands)//only affect CY
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char Rp = operands.first.front();
	int HL_DATA = Register::HL(), Rp_DATA = 0;
	if (Rp == 'H')
	{
		Rp_DATA = Register::HL();
	}
	else if (Rp == 'D')
	{
		Rp_DATA = Register::DE();
	}
	else if (Rp == 'B')
	{
		Rp_DATA = Register::BC();
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	HL_DATA += Rp_DATA;
	Register::Flag::CY = HL_DATA & (1 << 16);//@Checking for Carry
	Utility::_16Bit_Normalization(HL_DATA);
	std::string xDATA = Converter::DecToHex(HL_DATA, 16);
	Register::Main['H'] = Converter::HexToDec(xDATA.substr(0, 2));
	Register::Main['L'] = Converter::HexToDec(xDATA.substr(2, 2));
	++Register::PC;
}

//@Logical Instructions

void Mnemonic::ANA(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	if (reg == 'M')
	{
		Register::Main['A'] &= MemoryManager::Memory[Register::HL()];
	}
	else if (Register::isValid(reg))//TODO: Throw error if reg is A
	{
		Register::Main['A'] &= Register::Main[reg];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	Register::UpdateFlags(16);//16: As after executing AND instruction AC flag become set
	++Register::PC;
}

void Mnemonic::ANI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first);
	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		Register::Main['A'] &= nValue;
	}

	Register::UpdateFlags(16);//16: As after executing AND instruction AC flag become set
	++Register::PC;
}

void Mnemonic::ORA(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	if (reg == 'M')
	{
		Register::Main['A'] |= MemoryManager::Memory[Register::HL()];
	}
	else if (Register::isValid(reg))//TODO: Throw error if reg is A
	{
		Register::Main['A'] |= Register::Main[reg];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	Register::UpdateFlags(1);//1: As there can not be any auxiliary carry during bitwise operation
	++Register::PC;
}

void Mnemonic::ORI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first);

	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		Register::Main['A'] |= nValue;
	}

	Register::UpdateFlags(1);
	++Register::PC;
}

void Mnemonic::XRA(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	char reg = operands.first.front();
	if (reg == 'M')
	{
		Register::Main['A'] ^= MemoryManager::Memory[Register::HL()];
	}
	else if (Register::isValid(reg))//TODO: Throw error if reg is A
	{
		Register::Main['A'] ^= Register::Main[reg];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	Register::UpdateFlags(1);
	++Register::PC;
}

void Mnemonic::XRI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int nValue = Converter::HexToDec(operands.first);
	if (nValue > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}
	else
	{
		Register::Main['A'] ^= nValue;
	}

	Register::UpdateFlags(1);
	++Register::PC;
}

void Mnemonic::CMA(const std::pair<std::string, std::string>& operands)//Flags are not affected by this instruction
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	for (int i = 0; i < 8; ++i)
	{
		Register::Main['A'] ^= (1 << i);
	}
	++Register::PC;
}

void Mnemonic::RLC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::Flag::CY = Register::Main['A'] & (1 << 7);
	Register::Main['A'] <<= 1; //Left Shift by 1 bit
	Register::Main['A'] |= Register::Flag::CY ? 1 : 0;
	Utility::_8Bit_Normalization(Register::Main['A']);
	++Register::PC;
}

void Mnemonic::RAL(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	bool MSB = Register::Main['A'] & (1 << 7);
	Register::Main['A'] <<= 1;//Left Shift by 1 bit
	Register::Main['A'] |= Register::Flag::CY ? 1 : 0;
	Register::Flag::CY = MSB;
	Utility::_8Bit_Normalization(Register::Main['A']);
	++Register::PC;
}

void Mnemonic::RRC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::Flag::CY = Register::Main['A'] & 1;
	Register::Main['A'] >>= 1; //Right Shift by 1 bit
	Register::Main['A'] |= Register::Flag::CY ? (1 << 7) : 0;
	++Register::PC;
}

void Mnemonic::RAR(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	bool LSB = Register::Main['A'] & 1;
	Register::Main['A'] >>= 1;//Right Shift by 1 bit
	Register::Main['A'] |= Register::Flag::CY ? (1 << 7) : 0;
	Register::Flag::CY = LSB;
	++Register::PC;
}

void Mnemonic::STC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	Register::Flag::CY = 1;
	++Register::PC;
}

void Mnemonic::CMC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::Flag::CY = !Register::Flag::CY;
	++Register::PC;
}

void Mnemonic::CMP(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	char reg = operands.first.front();
	int A = Register::Main['A'], R = 0;
	if (reg == 'M')
	{
		R = MemoryManager::Memory[Register::HL()];
	}
	else if (Register::isValid(reg))
	{
		R = Register::Main[reg];
	}
	else
	{
		Error::Throw("Invalid Register", Register::PC);
	}

	int temp = A + Utility::_8bit_2sc(R);
	Utility::_8Bit_Normalization(temp);
	Register::Flag::PF = !(Utility::_set_bits_count(temp) & 1);

	int _4Bit_A = Converter::HexToDec(((Converter::DecToHex(A)).back()));
	int _4Bit_R = Converter::HexToDec(((Converter::DecToHex(Utility::_8bit_2sc(R))).back()));
	Register::Flag::AC = _4Bit_A + _4Bit_R > 15;

	Register::Flag::CY = A < R;
	Register::Flag::SF = A < R;
	Register::Flag::ZF = A == R;

	++Register::PC;
}

void Mnemonic::CPI(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}

	int A = Register::Main['A'], R = Converter::HexToDec(operands.first);

	if (R > 0xFF)
	{
		Error::Throw("Expected 8 Bit data", Register::PC);
	}

	int temp = A + Utility::_8bit_2sc(R);
	Utility::_8Bit_Normalization(temp);
	Register::Flag::PF = !(Utility::_set_bits_count(temp) & 1);

	int _4Bit_A = Converter::HexToDec(((Converter::DecToHex(A)).back()));
	int _4Bit_R = Converter::HexToDec(((Converter::DecToHex(Utility::_8bit_2sc(R))).back()));
	Register::Flag::AC = _4Bit_A + _4Bit_R > 15;

	Register::Flag::CY = A < R;
	Register::Flag::SF = A < R;
	Register::Flag::ZF = A == R;
	++Register::PC;
}

//@Branching Instructions
void Mnemonic::JMP(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Program::Loop[operands.first];
}

void Mnemonic::JC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::CY ? Program::Loop[operands.first] : Register::PC + 1;
}

void Mnemonic::JNC(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::CY ? Register::PC + 1 : Program::Loop[operands.first];
}

void Mnemonic::JZ(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::ZF ? Program::Loop[operands.first] : Register::PC + 1;
}

void Mnemonic::JNZ(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::ZF ? Register::PC + 1 : Program::Loop[operands.first];
}

void Mnemonic::JPE(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::PF ? Program::Loop[operands.first] : Register::PC + 1;
}

void Mnemonic::JPO(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::PF ? Register::PC + 1 : Program::Loop[operands.first];
}

void Mnemonic::JM(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::SF ? Program::Loop[operands.first] : Register::PC + 1;
}

void Mnemonic::JP(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 1))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Register::PC = Register::Flag::SF ? Register::PC + 1 : Program::Loop[operands.first];
}

void Mnemonic::HLT(const std::pair<std::string, std::string>& operands)
{
	if (!Mnemonic::validOperandCount(operands, 0))
	{
		Error::Throw("Invalid Operand Count", Register::PC);
		return;
	}
	Program::HLT = true;
}

//Function to print the lexer output to a file
void DebugLex()
{
	std::ofstream file;
	file.open("debug_lex.txt");
	for (Instruction& s : Program::program)
	{
		file << s.mnemonic << ' ' << s.operands.first << ' ' << s.operands.second << '\n';
	}
}


bool Program::Read(std::string filePath)
{
	Program::program.clear();//Clearing previous program
	Program::Loop.clear();//Clearing previous program

	std::fstream file;
	file.open(filePath, std::ios::in);
	while (!file.eof())
	{
		std::string currentLine;
		std::getline(file, currentLine);
		//Checking for commented line and empty line
		if (currentLine.empty() || currentLine.front() == '@')
		{
			continue;
		}

		std::stringstream ss(currentLine);
		std::string word;
		std::vector<std::string> tokens;
		while (ss >> word)
		{
			tokens.push_back(word);
		}
		
		int token_count = tokens.size();

		if (token_count > 5)
		{
			//We can not have more than 5 tokens in a line
			Error::Throw("Syntax error", program.size() + 1);
			return false;
		}
	
		Instruction instruction;

		int token_idx = 0;

		//First token can either be a loop point or a mnemonic
		if (tokens[token_idx].back() == ':') //If it is a loop
		{
			Converter::ToUpperString(tokens[token_idx]);
			Loop[tokens[token_idx].substr(0, tokens[token_idx].size() - 1)] = program.size();
			++token_idx;
		}

		//Checking for mnemonic
		if (token_idx < token_count)
		{
			Converter::ToUpperString(tokens[token_idx]);
			if (Mnemonic::isValid(tokens[token_idx]))
			{
				instruction.mnemonic = tokens[token_idx++];
			}
			else
			{
				Error::Throw("Invalid Mnemonic", program.size() + 1);
				return false;
			}
		}
		else
		{
			//This token should have to be a mnemonic
			Error::Throw("Syntax error", program.size() + 1);
			return false;
		}

		//Searching for first operand(comma may be attached to it. eg: A,)
		bool comma_found = false;
		if (token_idx < token_count)
		{
			std::string operand = tokens[token_idx];
			if (operand.back() == ',') // if comma is attaced to first operand
			{
				operand.erase(operand.end() - 1);//removing comma
				comma_found = true;
			}

			Converter::ToUpperString(operand);
			instruction.operands.first = operand;
			++token_idx;
		}
		else
		{
			program.push_back(instruction);
			continue;
			//as if there if no token after mnemonic then it must be a instruction with no operands
		}

		
		if (token_idx < token_count)
		{
			if (comma_found)//It should be a second operand
			{
				Converter::ToUpperString(tokens[token_idx]);
				instruction.operands.second = tokens[token_idx];
			}
			else
			{
				//Either the comma is separate from the operands (eg: A , B) or may be attaced to second operand(A ,B)
				std::string s = tokens[token_idx];
				if (s == ",")//Separated comma
				{
					//next token will be the second operand
					++token_idx;
					if (token_idx < token_count)
					{
						Converter::ToUpperString(tokens[token_idx]);
						instruction.operands.second = tokens[token_idx];
					}
					else
					{
						//No operand after a comma
						Error::Throw("Syntax error", program.size() + 1);
						return false;
					}
					
				}
				else
				{
					//It must be a attached comma
					if (s.front() == ',')
					{
						s.erase(s.begin());
						Converter::ToUpperString(s);
						instruction.operands.second = s;
					}
					else
					{
						Error::Throw("Syntax error", program.size() + 1);
						return false;
					}
				}
			}
		}
		else
		{
			program.push_back(instruction);
			continue;
			//as if there if no token after first operand then it must be a instruction with single operand
		}
		program.push_back(instruction);
	}

	if (program.back().mnemonic != "HLT")
	{
		Error::Throw("'HLT' Missing");
		return false;
	}
	else
	{
		return true;
	}
}

void Program::Run()
{
	HLT = false;
	while (!HLT)
	{
		const Instruction& instruction = program[Register::PC];
		Mnemonic::Execute[instruction.mnemonic](instruction.operands);
	}
}


