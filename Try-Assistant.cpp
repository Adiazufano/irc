#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <curl/curl.h>

#define HOST "127.0.0.1"
#define PORT 6667
#define PASSWORD "1234"
#define BOT_NICK "Omni-bot"
#define GROQ_URL "https://api.groq.com/openai/v1/chat/completions"
#define GROQ_KEY "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"
#define GROQ_MODEL "llama-3.1-8b-instant"


size_t writeAnswer(void *contents, size_t size, size_t nmemb, std::string *output)
{
    output->append((char *)contents, size *nmemb);
    return(size * nmemb);
}

// Hay que extraer el JSON manualmente ya que C++98 no tiene librería que lo haga
std::string extractJason(const std::string &json, const std::string &key)
{
    std::string search = "\"" + key + "\":\"";

    size_t pos = json.find(search);
    if(pos == std::string::npos)
        return("");
    pos += search.size();

    size_t end = json.find("\"", pos);
    if(end == std::string::npos)
        return("");
    return(json.substr(pos, end - pos));
}

std::string extractContent(const std::string &json)
{
    std::string search = "\"content\":\"";
    size_t pos = json.find(search);
    if(pos == std::string::npos)
        return("No reponse");
    
    pos += search.size();
    std::string result;
    while(pos < json.size() && json[pos] != '"')
    {
        if(json[pos] == '\\' && pos + 1 < json.size())
        {
            pos++; // Nos saltamos el backslash
            if(json[pos] == 'n')
                result += ' ';  // reemplazamos los saltos de lineas con esapcios ya que IRC escribe todo en una línea.
            else if(json[pos] == '"')
                result += '"';
            else
                result += json[pos];
        }
        else
            result += json[pos];
        pos++;
    }
    return (result);
}


std::string solverAI(std::string& question)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return ("Error initializing curl");

    std::string response;
    std::string jsonBody = "{\"model\":\"" + std::string(GROQ_MODEL) + "\"," + "\"messages\":[{\"role\":\"user\",\"content\":\"" + question + "\"}]}";

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + std::string(GROQ_KEY)).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, GROQ_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeAnswer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK)
        return("Error: " + std::string(curl_easy_strerror(res)));

    std::cout << "Groq response: " << response << std::endl; // debug
    return (extractContent(response));
}


void sendMsg(int fd, std::string msg)
{
    msg += "\r\n";
    send(fd, msg.c_str(), msg.size(), 0);
}


std::string parseQuestion(const std::string& question)
{
    std::string parsed;

    for(size_t i = 0; i < question.size(); i++)
    {
        if(question[i] == '"')
            parsed += "\\\"";
        else if(question[i] == '\\')
            parsed += "\\\\";
        else if(question[i] == '\n')
            parsed += ' ';
        else if(question[i] == '\r')
            parsed += ' ';
        else
            parsed += question[i];
    }
    return(parsed);
}


std::string handlePrivmsg(std::string sender, std::string target, std::string msg)
{
    std::string responseTarget;

    if(target == BOT_NICK)
        responseTarget = sender;
    else
        responseTarget = target;

    // Cortamos el mensaje para eliminar los escapes del final
    while(!msg.empty() && (msg[msg.size() - 1] == '\r' || msg[msg.size() - 1] == '\n' || msg[msg.size() - 1] == ' '))
        msg.erase(msg.size() - 1);

    if(msg.substr(0, 4) == "!ask")
    {
        std::string question = parseQuestion((msg.substr(5) + " Please condense your answer to 400 characters or less if possible."));
        std::string answer = solverAI(question);
        if(answer.size() > 400)
            answer = answer.substr(0, 400) + "...";
        return("PRIVMSG " + responseTarget + " :" + answer);
    }
    return ("PRIVMSG " + responseTarget + " :Unknown command. Use !ask <question>");
}


int main()
{
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        std::cerr << "Socket error" << std::endl;
        return 1;
    }

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;			// IPv4
	addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &addr.sin_addr);

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Connect error" << std::endl;
        close (fd);
        return (1);
    }

    bool registered = false;

    sendMsg(fd, "PASS " + std::string(PASSWORD));
    sendMsg(fd, "NICK " + std::string(BOT_NICK));
    sendMsg(fd, "USER Omni-bot 0 * :Omni-bot");
    sendMsg(fd, "CAP END");

    char buffer[2048];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if(bytes <= 0)
            break ;

        std::string data(buffer);
        std::cout << data << std::endl;

        if(data.substr(0, 4) == "PING")
            sendMsg(fd, "PONG " + data.substr(5));

        if(!registered && data.find("001") != std::string::npos)
        {
            sendMsg(fd, "JOIN #Omni-bot");
            registered = true;
        }

        if(data.find("PRIVMSG") != std::string::npos && data[0] == ':')
        {
            try
            {
                std::string sender = data.substr(1, data.find('!') - 1);
                std::string afPrivmsg = data.substr(data.find("PRIVMSG") + 8);
                std::string target = afPrivmsg.substr(0, afPrivmsg.find(' '));
                std::string msg = afPrivmsg.substr(afPrivmsg.find(':') + 1);

                if(sender != BOT_NICK)
                    sendMsg(fd, handlePrivmsg(sender, target, msg));
            }
            catch(std::exception &e)
            {
                std::cerr << "Parse error: " << e.what() << std::endl;
            }
        }
    }
    close (fd);
    return (0);
}