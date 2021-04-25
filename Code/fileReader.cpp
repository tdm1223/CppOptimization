#include<iostream>
#include<string>
#include<sstream>
#include<fstream>

std::string file_reader(char const* fname)
{
    std::ifstream f;
    f.open(fname);
    if (!f)
    {
        std::cout << "Can't open " << fname << " for reading" << std::endl;
        return "";
    }

    std::stringstream s;
    std::copy(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(s));
    return s.str();
}

void stream_read_streambuf_stringstream(std::istream& f, std::string& result)
{
    std::stringstream s;
    std::copy(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(s));
    s.str().swap(result);
}

void stream_read_streambuf_string(std::istream& f, std::string& result)
{
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}

void stream_read_streambuf(std::istream& f, std::string& result)
{
    std::stringstream s;
    s << f.rdbuf();
    s.str().swap(result);
}

void stream_read_string_reserve(std::istream& f, std::string& result)
{
    f.seekg(0, std::istream::end);
    std::streamoff len = f.tellg();
    f.seekg(0);
    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}

void stream_read_string_2(std::istream& f, std::string& result, std::streamoff len = 0)
{
    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }
    result.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
}

void stream_read_getline(std::istream& f, std::string& result)
{
    std::string line;
    result.clear();
    while (getline(f, line))
    {
        (result += line) += "\n";
    }
}

void stream_read_getline_2(std::istream& f, std::string& result, std::streamoff len = 0)
{
    std::string line;
    result.clear();

    if (len > 0)
    {
        result.reserve(static_cast<std::string::size_type>(len));
    }

    while (getline(f, line))
    {
        (result += line) += "\n";
    }
}

std::streamoff stream_size(std::istream& f)
{
    std::istream::pos_type current_pos = f.tellg();
    if (-1 == current_pos)
    {
        return -1;
    }
    f.seekg(0, std::istream::end);
    std::istream::pos_type end_pos = f.tellg();
    f.seekg(current_pos);
    return end_pos - current_pos;
}

bool stream_read_sgetn(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == 01)
    {
        return false;
    }

    result.resize(static_cast<std::string::size_type>(len));

    f.rdbuf()->sgetn(&result[0], len);
    return true;
}

bool stream_read_string(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == -1)
    {
        return false;
    }
    result.resize(static_cast<std::string::size_type>(len));
    f.read(&result[0], result.length());
    return true;
}

bool stream_read_array(std::istream& f, std::string& result)
{
    std::streamoff len = stream_size(f);
    if (len == -1)
    {
        return false;
    }

    std::unique_ptr<char> data(new char[static_cast<size_t>(len)]);
    f.read(data.get(), static_cast<std::streamsize>(len));
    result.assign(data.get(), static_cast<std::string::size_type>(len));
    return true;
}