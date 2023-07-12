#include "Curl.hpp"
string LoginURL = "https://cas2.edu.sh.cn/CAS/login?service=https%3A%2F%2Fsmilelogin.shec.edu.cn%2Fsmile-login%2Fauth%2Fcas%2Flogin%3Ftarget%3Dhttps%3A%2F%2Fsmile.shec.edu.cn%2F%3Fpath%3DairClassroomPage";
int main(int argc, char **argv)
{
    CLN_TRY
    string Username = "";
    string Password = "";
    string GradeID = "";
    string SemesterNo = "";
    string Subject = "";
    for (int i = 1; i < argc; i++)
    {
        string Argument = argv[i];
        string NextArgument = i + 1 == argc ? "" : argv[i + 1];
        if (Argument == "-u" || Argument == "--username")
        {
            Username = NextArgument;
            i++;
        }
        else if (Argument == "-p" || Argument == "--password")
        {
            Password = NextArgument;
            i++;
        }
        else if (Argument == "-g" || Argument == "--grade")
        {
            GradeID = NextArgument;
            i++;
        }
        else if (Argument == "-s" || Argument == "--semester")
        {
            SemesterNo = NextArgument;
            i++;
        }
        else if (Argument == "-sj" || Argument == "--subject")
        {
            Subject = NextArgument;
            i++;
        }
        else
            TRIGGER_ERROR("Unknown option \"" + Argument + "\"");
    }
    if (Username == "")
        TRIGGER_ERROR("No username provided");
    if (Password == "")
        TRIGGER_ERROR("No password provided");
    if (GradeID == "")
        TRIGGER_ERROR("No grade provided");
    if (SemesterNo == "")
        TRIGGER_ERROR("No semester provided");

    cout << "Checking login... " << flush;
    int HTTPResponseCode = 0;
    GetDataToFile(LoginURL, "", "", false, "", NULL, &HTTPResponseCode);
    if (HTTPResponseCode == 200)
    {
        cout << "Not logged in" << endl
             << "Logging in... " << flush;
        GetDataToFile(LoginURL,
                      "",
                      "",
                      true,
                      "username=" + Username +
                          "&password=" + Password +
                          "&role=1001&openid=&execution=" +
                          GetStringBetween(GetDataFromFileToString(),
                                           "<input type=\"hidden\" id=\"execution_id\" name=\"execution\" value=\"",
                                           "\"") +
                          "&checkFlag=temp&_eventId=submit",
                      NULL,
                      &HTTPResponseCode,
                      "application/x-www-form-urlencoded");
        if (HTTPResponseCode != 302)
        {
            cout << "Failed" << endl;
            return 0;
        }
        cout << "Succeed" << endl;
    }
    else
        cout << "Already logged in" << endl;
    cout << "Redirect login... " << flush;
    string RedirectURL = FindLocation();
    GetDataToFile(RedirectURL);
    cout << "Success" << endl
         << "Getting class list size... " << flush;
    json ClassesRequest;
    ClassesRequest["start"] = 1;
    ClassesRequest["length"] = 1;
    ClassesRequest["showNotStudy"] = 1;
    ClassesRequest["sectionId"] = 2;
    ClassesRequest["smileSubjectId"] = "";
    ClassesRequest["gradeId"] = GradeID;
    ClassesRequest["semesterNo"] = SemesterNo;
    GetDataToFile("https://www.sh.smartedu.cn/smile-index-service/api/index/registerResource/pageResource",
                  "",
                  "",
                  true,
                  ClassesRequest.dump());
    json Classes = json::parse(GetDataFromFileToString());
    ClassesRequest["length"] = atoi(Classes["data"]["total"].as_string().c_str());
    cout << "Success" << endl
         << "Getting class list... " << flush;
    GetDataToFile("https://www.sh.smartedu.cn/smile-index-service/api/index/registerResource/pageResource",
                  "",
                  "",
                  true,
                  ClassesRequest.dump());
    cout << "Success" << endl
         << "Parsing data... " << flush;
    Classes = json::parse(GetDataFromFileToString());
    queue<pair<string, string>> VideoList;
    for (auto i : Classes["data"]["list"])
    {
        string FileName = i["subjectName"].as_string() + "_" +
                          i["finalPremiereTime"].as_string() + "_" +
                          i["unitName"].as_string() + "_" +
                          i["name"].as_string() + ".mp4";
        FileName = StringReplaceAll(FileName, "/", "-");
        if (i["subjectName"].as_string().find(Subject) != string::npos && !IfFileExist("/home/langningc2009/AirClassroomDownload/" + FileName))
            VideoList.push({FileName, i["encryptUrl"].as_string()});
    }
    int Size = VideoList.size();
    cout << "Success" << endl;
    while (!VideoList.empty())
    {
        cout << "\033cDownloading "
             << Size - VideoList.size() + 1 << "/" << Size << "  " << VideoList.front().first << endl;
        if (1)
        {
            GetDataToFile(VideoList.front().second,
                          "",
                          VideoList.front().first,
                          false,
                          "",
                          NULL,
                          NULL,
                          "application/json",
                          "",
                          true);
        }
        else
        {
            system(string("\"/mnt/c/Program Files (x86)/Internet Download Manager/idman.exe\" /d \"" + VideoList.front().second + "\" /p \"D:\" /f \"" + VideoList.front().first + "\"").c_str());
            sleep(13);
        }
        VideoList.pop();
    }
    CLN_CATCH
    return 0;
}
