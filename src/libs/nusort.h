
#include <stdlib.h>

#include <ctype.h>
int strnucmp(short int *ac,const char * str1,const char * str2)
{
//if (!*ac){
//*ac=-1;
//}
	size_t numlength=0;
	while(1)
	{
out:
		if(*str1=='\0' && !(*str2=='\0'))
		{
			return -*ac;
		}
		if(!(*str1=='\0') && *str2=='\0')
		{
			return *ac;
		}
		if(*str1=='\0' && *str2=='\0')
		{
			return 0;
		}
		if(isdigit(*str1)&&isdigit(*str2))
		{
			while(1)
			{
				if(isdigit(*str1)&& !isdigit(*str2))
				{
					return *ac;
				}
				if(!isdigit(*str1)&&isdigit(*str2))
				{
					return -*ac;
				}
				if(!isdigit(*str1)&&!isdigit(*str2))
				{
					str1-=numlength;
					str2-=numlength;
					while(1)
					{
						if(!isdigit(*str1)&&!isdigit(*str2))
						{
							numlength=0;
							goto out;
						}
						if(*str1>*str2)
						{
							return *ac;
						}
						if(*str1<*str2)
						{
							return -*ac;
						}
						str1 ++;
						str2 ++;
					}
				}
				str1++;
				str2++;
				numlength++;
			}
		}
		else if(isdigit(*str1)&& !isdigit(*str2))
		{
			return -*ac;
		}
		else if(!isdigit(*str1)&&isdigit(*str2))
		{
			return *ac;
		}
		else
		{
			if(*str1>*str2)
			{
				return *ac;
			}
			if(*str1<*str2)
			{
				return -*ac;
			}
		}
		str1++;
		str2++;
	}
}
