String webpage = ""; //String to save the html code

void append_page_header() {
  webpage  = F("<!DOCTYPE html><html>");
  webpage += F("<head>");
  webpage += F("<title>Server</title>"); // NOTE: 1em = 16px
  webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
  webpage += F("<style>");//From here style:

  webpage += F(".dashboard{margin:0 20px 0 20px;}");////main table

  webpage += F(".form-upload{text-align:center;}");///uploading form
  webpage += F(".upload-btn{color:black;diaplay:block;padding:10px;border-radius:8px;padding:0.44em 0.44em;text-decoration:none;font-size:100%;border: 1px solid #0c3c54;margin: 0 0 0 5px;transition:background-color 0.2s;}");
  webpage += F(".upload-btn:hover{background-color:#0c3c54;color: #ffffff;border-radius: 8px;font-size:100%}");
  webpage += F("input::file-selector-button{font-weight: bold;border-radius: 5px;padding: 5px;cursor: pointer; }");//upload input
  webpage += F("input{width:25%;}");

  webpage += F(".h-text{text-align:center;font-size: 30px;margin:0 0 10px 0;}");//H tag for centre
  //all back button
  webpage += F(".back{position: absolute;transform: translate(20%);font-family:'McLaren', cursive;border-radius:8px;padding:8px 15px;text-decoration:none;border: 1px solid #0c3c54;margin: 0 0 0 5px;background-color:#8d9db6;transition:background-color 0.2s;color:white;}");
  webpage += F(".back:hover{color: #ffffff;border-radius: 8px;background-color:#ffffff;color:black;}");
  
  webpage += F("ul{list-style-type:none;margin: 20px 20px;padding:0;height: 3em;border-radius:8px;overflow:hidden;box-shadow: 0 10px 6px -6px #312e2e;}");
  webpage += F("li{float:left;border-radius:8px;margin-top: 10px;border-right:0em solid #bbb;}");
  webpage += F("li a{color:black;diaplay:block;border-radius:8px;padding:0.44em 0.44em;text-decoration:none;font-size:100%;border: 1px solid #0c3c54;margin: 0 0 0 5px;transition:background-color 0.2s}");
  webpage += F("li a:hover{background-color:#0c3c54;color: #ffffff;border-radius: 8px;font-size:100%;transition:background-color 0.2s;}");
  
  webpage += F(".download, .delete{color:black;diaplay:block;border-radius:8px;padding:0.44em 0.44em;text-decoration:none;transition:background-color 0.2s;font-size:100%;border: 1px solid #0c3c54;margin: 0 0 0 5px;}");
  webpage += F(".download:hover{background-color:green;color: #ffffff;border-radius: 8px;font-size:100%}");
  webpage += F(".delete:hover{background-color:red;color: #ffffff;border-radius: 8px;font-size:100%}");
  
  webpage += F("h1{color: rgb(24, 66, 101);font-size:1.6em;height:3em;padding:10px;text-align: center;border-radius: 0 0 8px 8px;box-shadow: 0 10px 6px -6px #312e2e;}");
  webpage += F("h2{color: green;font-size:0.8em;}");
  
  webpage += F("table{background-color:#cbcbcb7a;border-radius: 10px;border-collapse:collapse;width:100%;-webkit-box-shadow: 0 10px 6px -6px #312e2e;box-shadow: 0 10px 6px -6px #312e2e;}"); 
  webpage += F("th,td {text-align:left;padding:0.3em;border:1px solid white;}"); 
  
  webpage += F("*{box-sizing:border-box;margin:0;font-family:'McLaren', cursive;}");
  webpage += F("a{font-size:75%;}");
  webpage += F("p{font-size:75%;}");
  webpage += F("</style></head><body><h1 >ESP32 Server</h1>");
  webpage += F("<ul>");
  webpage += F("<li><a href='/'>Files</a></li>"); //Menu bar with commands
  webpage += F("<li><a href='/upload'>Upload File</a></li>"); 
  webpage += F("</ul>");
}
//Saves repeating many lines of code for HTML page footers
void append_page_footer()
{ 
  webpage += F("</body></html>");
}
