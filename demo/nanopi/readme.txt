������ʹ�ã�bug �ܶ�

ʵ�ֹ���: �ֻ��˷��� �ַ��� �� ������LCD ����ʾ

ʹ�÷�����
1������ ./android-clinet Ŀ¼�µ� client �ļ��� system/bin/
2������ ./nanopi-server Ŀ¼�µ� matrix-lcd1602_keypad ��������� /usr/bin/

3���������ϵ磬ִ�� /usr/bin/matrix-lcd1602_keypad
   ��ʾ waiting key press... ��ʱ���� F1, �����彫��ʾ IP �� �˿ں�
   
4���ֻ��� 
   adb shell
   cd system/bin/
   ./client   
   
5����ʱ�ֻ��˳ɹ������Ͽ����壬�ֻ�����ʾ Welcome to nanopi
6���ֻ�����ʾ Enter string to send: ��ʱ������ʾ�ַ����� hello����������ʾ������ʾhello


