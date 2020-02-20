{if (($1 == "var-list") || ($1 == "block") || ($1 == "record") || ($1 == "unit") ||
($1 == "array") || ($1 == "menu") || ($1 == "axis") || ($1 == "chart") ||
($1 == "waveform") || ($1 == "method") || ($1 == "grid") || ($1 == "image")){
if (substr($3,1,3) != "0x8") { printf("%s\n", $0)}}
else if ((substr($4,1,3) != "0xC") && (substr($4,1,3) != "0x8")) { printf("%s\n", $0)}}
