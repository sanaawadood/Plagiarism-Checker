#include<bits/stdc++.h>
using namespace std;

#include <string.h>  /* for memcmp and memmove */
#include <stdlib.h>      /* for malloc, free */
#include <ctype.h>       /* for isupper, islower, tolower */
#define LETTER(ch) (isupper(ch) || islower(ch))
#define INC 50

static char *b;       // buffer for word to be stemmed 
static int k,k0,j;     // j is a general offset into the string 

static int cons(int i){		// cons(i) is TRUE if b[i] is a consonant.   
	switch (b[i]){  
   		case 'a': case 'e': case 'i': case 'o': case 'u': return 0;
    	case 'y': return (i==k0) ? 1 : !cons(i-1);
      	default: return 1;
   	}
}

static int m(){	//measures the number of vc
  	int n = 0,i = k0;
   	while(1){  
	   	if 	(i > j) return n;
   	   	if 	(!cons(i)) break; 
		i++;
	}
   	i++;
   	while(1){
	   	while(1){  
			if (i > j) return n;
	        if (cons(i)) break;
	    	i++;
   		}
	   	i++;
	   	n++;
	    while(1){  
	   		if (i > j) return n;
	   	    if (!cons(i)) break;
			i++;
	   	}
	   	i++;
   	}
}

static int vowelinstem(){  	// TRUE when k0,...j contains a vowel 
	int i; 
	for (i = k0; i <= j; i++) 
		if (!cons(i))	return 1;
   	return 0;
}

static int doublec(int j){	// TRUE if j,(j-1) contain a double consonant.   
	if (j < k0+1)	return 0;
   	if (b[j] != b[j-1])	  return 0;
   	return cons(j);
}

static int cvc(int i){		//this is used when trying to restore an e at the end of a short word.  
	if (i < k0+2 || !cons(i) || cons(i-1) || !cons(i-2)) return 0;{  
   		int ch = b[i];
      	if (ch == 'w' || ch == 'x' || ch == 'y') return 0;
   	}
   	return 1;
}

static int ends(char *s){ 		// TRUE if k0,...k ends with the string s. 
	int length = s[0];
	if (s[length] != b[k]) return 0; 
   	if (length > k-k0+1) return 0;
   	if (memcmp(b+k-length+1,s+1,length)) return 0;
   	j = k-length;
   	return 1;
}

static void setto(char * s){ 	//sets (j+1),...k to the characters in the string s, readjusting k. 
	int length = s[0];
   	memmove(b+j+1,s+1,length);
   	k = j+length;
}

static void r(char * s) {
	if (m() > 0) setto(s); 
}	

static void step1ab(){	//gets rid of plurals and -ed or -ing.
	if (b[k] == 's'){  
	   	if (ends("\04" "sses")) k -= 2; 
		else if (ends("\03" "ies")) setto("\01" "i");
      	else if (b[k-1] != 's') k--;
   	}
   	if (b[k] == 'y'){  
	   	if (ends("\03" "day")) 	setto("\00" "");	///TO CHANGE MONDAY TUESD....TO MON TUES ...
   	}
   	if (ends("\03" "eed")) { if (m() > 0) k--; } else
   	if ((ends("\02" "ed") || ends("\03" "ing")) && vowelinstem())
   	{  
	   	k = j;
        if (ends("\02" "at")) setto("\03" "ate"); 
      	else if (ends("\02" "bl")) setto("\03" "ble");
      	else if (ends("\02" "iz")) setto("\03" "ize");
      	else if (doublec(k)){
			k--;
         	{
			   	int ch = b[k];
            	if (ch == 'l' || ch == 's' || ch == 'z') k++;
         	}
      	}
      	else if (m() == 1 && cvc(k)) setto("\01" "e");
   	}
}

static void step1c() {		///* turns terminal y to i when there is another vowel in the stem.  
	if (ends("\01" "y") && vowelinstem()) 
		b[k] = 'i'; 
}

static void step2() { 		//  maps double suffices to single ones. so -ization ( = -ize plus -ation) maps to -ize etc.
	switch (b[k-1]){
    case 'a': if (ends("\07" "ational")) { r("\03" "ate"); break; }
              if (ends("\06" "tional")) { r("\04" "tion"); break; }
              break;
    case 'c': if (ends("\04" "enci")) { r("\04" "ence"); break; }
              if (ends("\04" "anci")) { r("\04" "ance"); break; }
              break;
    case 'e': if (ends("\04" "izer")) { r("\03" "ize"); break; }
              break;
    case 'l': if (ends("\03" "bli")) { r("\03" "ble"); break; }
              if (ends("\04" "alli")) { r("\02" "al"); break; }
              if (ends("\05" "entli")) { r("\03" "ent"); break; }
              if (ends("\03" "eli")) { r("\01" "e"); break; }
              if (ends("\05" "ousli")) { r("\03" "ous"); break; }
              break;
    case 'o': if (ends("\07" "ization")) { r("\03" "ize"); break; }
              if (ends("\05" "ation")) { r("\03" "ate"); break; }
              if (ends("\04" "ator")) { r("\03" "ate"); break; }
              break;
    case 's': if (ends("\05" "alism")) { r("\02" "al"); break; }
              if (ends("\07" "iveness")) { r("\03" "ive"); break; }
              if (ends("\07" "fulness")) { r("\03" "ful"); break; }
              if (ends("\07" "ousness")) { r("\03" "ous"); break; }
              break;
    case 't': if (ends("\05" "aliti")) { r("\02" "al"); break; }
              if (ends("\05" "iviti")) { r("\03" "ive"); break; }
              if (ends("\06" "biliti")) { r("\03" "ble"); break; }
              break;
    case 'g': if (ends("\04" "logi")) { r("\03" "log"); break; } 
	} 
}

static void step3() { 	///deals with -ic-, -full, -ness etc. with strategy to step2. 
	switch (b[k]){
    case 'e': 
			if (ends("\05" "icate")) { r("\02" "ic"); break; }
            if (ends("\05" "ative")) { r("\00" ""); break; }
            if (ends("\05" "alize")) { r("\02" "al"); break; }
            break;
    case 'i':
			if (ends("\05" "iciti")) { r("\02" "ic"); break; }
            break;
    case 'l':
			if (ends("\04" "ical")) { r("\02" "ic"); break; }
            if (ends("\03" "ful")) { r("\00" ""); break; }
        	break;
    case 's':
			if (ends("\04" "ness")) { r("\00" ""); break; }
            break;
	} 
}

static void step4()		///*takes off -ant, -ence etc., in context <c>vcvc<v>. 
{  switch (b[k-1])
    {  case 'a': if (ends("\02" "al")) break; 
					return;
       case 'c': if (ends("\04" "ance")) break;
                 if (ends("\04" "ence")) break; 
				return;
       case 'e': if (ends("\02" "er")) break; 
	   				return;
       case 'i': if (ends("\02" "ic")) break;
	   			 return;
       case 'l': if (ends("\04" "able")) break;
                 if (ends("\04" "ible")) break; 
				return;
       case 'n': if (ends("\03" "ant")) break;
                 if (ends("\05" "ement")) break;
                 if (ends("\04" "ment")) break;
                 if (ends("\03" "ent")) break;
				 return;
       case 'o': if (ends("\03" "ion") && j >= k0 && (b[j] == 's' || b[j] == 't')) break;
                 if (ends("\02" "ou")) break; 
				 return;
       case 's': if (ends("\03" "ism")) break; 
	   			 return;
       case 't': if (ends("\03" "ate")) break;
                 if (ends("\03" "iti")) break; 
				 return;
       case 'u': if (ends("\03" "ous")) break; 
	   			 return;
       case 'v': if (ends("\03" "ive")) break; 
	   			 return;
       case 'z': if (ends("\03" "ize")) break; 
	   			 return;
       default: return;
    }
    if (m() > 1) k = j;
}

static void step5(){ 	///* removes a final -e if m() > 1, and changes -ll to -l if m() > 1 
	j = k;
   	if (b[k] == 'e'){
	    int a = m();
      	if (a > 1 || a == 1 && !cvc(k-1)) k--;
   	}
   	if (b[k] == 'l' && doublec(k) && m() > 1) k--;
}

int stem(char * p, int i, int j){  
	b = p; k = j; k0 = i;
   	if (k <= k0+1) return k;
	step1ab();
   	if (k > k0) {
       	step1c(); 	step2(); 	step3();	 step4();	 step5();
   	}
   	return k;
}
static char * s;         // a char * pointer passed into b above
static int i_max = INC;

void increase_s(){  
	i_max += INC;
   	{
	    char * new_s = (char *) malloc(i_max+1);
      	{ 
		  	int i; 
			for (i = 0; i < i_max; i++) 
				new_s[i] = s[i];
		}
      	free(s); 
		s = new_s;
   	}
}


vector < map < string,double > > all_freq;	///each words frequencies in separate maps
int total_count=0;
double moduluss[4009],cosine_matrix[4009][4009];
map < string,int > freq;
map < string,double > freq_norm;	
set<string> all_word;	///all words
string ss;

int main(){
	#ifndef TRY
        //freopen("input.txt","r",stdin);
        freopen("output.txt","w",stdout);
    #endif
    
	FILE *fp,*fp_ans;
	int total_files=2;
	char number[100],name[10];
	
	for(int countr=1;countr<=total_files;countr++){		///Running all files
		
		strcpy(name,"doc-");
		itoa(countr,number,10);
		strcat(name,number); 
		fp=fopen(name,"r");		if(fp==NULL) return 0;			/////opening file
		fp_ans=fopen("result.txt","w");	
		char c[50],tt[50];
		int total_stopword=0,i=0,tottt=0;
		//Checking for each word is stopword or not
		while(fscanf(fp,"%s",tt)!=EOF) {
			
			FILE *temp;
			int check=0;
			temp=fopen("stopword.txt","r");
			if(temp==NULL) { cout << "no stopword.txt found\n"; return 0;}
			tottt = 0;
			while(fscanf(temp,"%s",c)!=EOF){	//checking each word in stopword.txt file
				if(!strcmp(c,tt)){
					check=1;
					total_stopword++;
					break;
				}
				tottt++;
			}
			if(!check)	
				fprintf(fp_ans,"%s ",tt);
			
			fclose(temp);
		}
		fclose(fp_ans);
		//cout << tottt<<endl;
				
		//	------ stemming part begins.. ...
		fp_ans=fopen("result.txt","r");		//stemming on result file() obtained after stopword operations above	
	   	s = (char*)malloc(i_max+1);
	   	freq.clear();
	   	freq_norm.clear();
	   	total_count=0;
	   	
	    while(1){
	    	int ch = getc(fp_ans);
	      	if (ch == EOF) break;
	      	if (LETTER(ch)){
		
				int i = 0;
		       	while(1){  	
				   	if (i == i_max) increase_s();
		           	ch = tolower(ch);
		           	s[i] = ch; i++;
		            ch = getc(fp_ans);
		            if (!LETTER(ch)) { ungetc(ch,fp_ans); break; }
		        }
		        s[stem(s,0,i-1)+1] = 0;
		        ss="";
		        
				for(int ii=0;ii<strlen(s);ii++)
		        	ss+=(char)s[ii];
		        
				if(ss.size()>1){
		        	all_word.insert(ss);
					freq[ss]++;
					total_count++;
				}		    	
	      	}
	   	}
	   	
	   	for(auto i : freq)
			cout << i.first << " " << i.second<<endl;
		cout << "\n\n";
	   	
	   	for(map < string,int >::iterator it=freq.begin();it!=freq.end();it++){
			
			freq_norm[(*it).first]= (double)((*it).second) / (double)total_count;		//Normalized TF for Doc in freq_norm map...
		}	   	
	   	all_freq.push_back(freq_norm);
	   	
	   	fclose(fp);
		fclose(fp_ans);
	}
	/*for(auto i : all_freq[0])
		cout << i.first << " " << i.second<<endl;
	cout <<"anouther one\n\n";
	for(auto i : all_freq[1])
		cout << i.first << " " << i.second<<endl;*/
		
	//MODULUS calc...
	for(int i = 0; i < total_files; i++){
		
		double sum=0;
		for(map<string,double>::iterator it=all_freq[i].begin();it!=all_freq[i].end();it++)
			sum+=( (*it).second * (*it).second );
			
		moduluss[i]=(double)sqrt(sum);
	}
	//COSINE calc...
	set < pair<int,int> > chker;
	double maxx=0.000;
	int f1=1,f2=1;
	
	for(int i = 0; i < total_files; i++){
		for(int j = 0; j < total_files; j++){

			if(chker.find(make_pair(j,i))!=chker.end()){	//if already calculated
				printf("%lf  ",cosine_matrix[j][i]);
				continue;
			}
			if(i == j){
				cosine_matrix[i][i] = 100.00; 
				printf("%lf  ",cosine_matrix[i][i]);
				continue;
			}
			
			double sum=0;
			for(map<string,double>::iterator it=all_freq[i].begin();it!=all_freq[i].end();it++){
				
				if(all_freq[j].find((*it).first)!=all_freq[j].end())
					sum+=( (*it).second * all_freq[j][(*it).first] );
			}
			
			cosine_matrix[i][j] = cosine_matrix[j][i] = (sum / (moduluss[i] * moduluss[j])) * 100.00 ;
			if ( maxx < cosine_matrix[i][j] ){	//checking for maximum
				f1=i+1; f2=j+1;
				maxx=cosine_matrix[i][j];
			}
			printf("%lf  ",cosine_matrix[i][j]);
			chker.insert(make_pair(i,j));
		}
		cout << "\n\n";
	}
	printf("\n\nMaximum : %lf%\nBetween files %d and %d\n",maxx,f1,f2);

	return 0;
}

