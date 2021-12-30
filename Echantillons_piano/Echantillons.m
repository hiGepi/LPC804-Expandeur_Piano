a = 1;

load A4.txt
A4 = A4(:,:);
A4 = round(a*0.536*2*5120*A4+511);

load A4d.txt
A4d = A4d(:,:);
A4d = round(a*2*5120.*A4d+511);

load B4.txt
B4 = B4(:,:);
B4 = round(a*1.408*0.536*2*5120*B4+511);

load C4.txt
C4 = C4(:,:);
C4 = round(a*2.09*0.536*2*5120*C4+511);

load C4d.txt
C4d = C4d(:,:);
C4d = round(a*2.21*0.536*2*5120*C4d+511);

load D4.txt
D4 = D4(:,:);
D4 = round(a*1.49*0.536*2*5120*D4+511);

load D4d.txt
D4d = D4d(:,:);
D4d = round(a*1.12*0.536*2*5120*D4d+511);

load E4.txt
E4 = E4(:,:);
E4 = round(a*0.536*2*5120*E4+511);

load F4.txt
F4 = F4(:,:);
F4 = round(a*1.41*0.536*2*5120*F4+511);

load F4d.txt
F4d = F4d(:,:);
F4d = round(a*2.09*0.536*2*5120*F4d+511);

load G4.txt
G4 = G4(:,:);
G4 = round(a*2.14*0.536*2*5120*G4+511);

load G4d.txt
G4d = G4d(:,:);
G4d = round(a*1.585*0.536*2*5120*G4d+511);

fileData = fopen('piano.txt','w');

fprintf(fileData,'NbEchantillon = {%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}\r\n\n',length(C4),length(C4d),length(D4),length(D4d),length(E4),length(F4),length(F4d),length(G4),length(G4d),length(A4),length(A4d),length(B4));

fprintf(fileData,'C4 = {');
for i = 1:length(C4)-1
    fprintf(fileData,'%d,',C4(i));
end
fprintf(fileData,'%d',C4(length(C4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'C4d = {');
for i = 1:length(C4d)-1
    fprintf(fileData,'%d,',C4d(i));
end
fprintf(fileData,'%d',C4d(length(C4d)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'D4 = {');
for i = 1:length(D4)-1
    fprintf(fileData,'%d,',D4(i));
end
fprintf(fileData,'%d',D4(length(D4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'D4d = {');
for i = 1:length(D4d)-1
    fprintf(fileData,'%d,',D4d(i));
end
fprintf(fileData,'%d',D4d(length(D4d)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'E4 = {');
for i = 1:length(E4)-1
    fprintf(fileData,'%d,',E4(i));
end
fprintf(fileData,'%d',E4(length(E4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'F4 = {');
for i = 1:length(F4)-1
    fprintf(fileData,'%d,',F4(i));
end
fprintf(fileData,'%d',F4(length(F4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'F4d = {');
for i = 1:length(F4d)-1
    fprintf(fileData,'%d,',F4d(i));
end
fprintf(fileData,'%d',F4d(length(F4d)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'G4 = {');
for i = 1:length(G4)-1
    fprintf(fileData,'%d,',G4(i));
end
fprintf(fileData,'%d',G4(length(G4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'G4d = {');
for i = 1:length(G4d)-1
    fprintf(fileData,'%d,',G4d(i));
end
fprintf(fileData,'%d',G4d(length(G4d)));
fprintf(fileData,'}\r\n');


fprintf(fileData,'A4 = {');
for i = 1:length(A4)-1
    fprintf(fileData,'%d,',A4(i));
end
fprintf(fileData,'%d',A4(length(A4)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'A4d = {');
for i = 1:length(A4d)-1
    fprintf(fileData,'%d,',A4d(i));
end
fprintf(fileData,'%d',A4d(length(A4d)));
fprintf(fileData,'}\r\n');

fprintf(fileData,'B4 = {');
for i = 1:length(B4)-1
    fprintf(fileData,'%d,',B4(i));
end
fprintf(fileData,'%d',B4(length(B4)));
fprintf(fileData,'}\r\n');

fclose(fileData);

hold on;
plot(C4);
plot(C4d);
plot(D4);
plot(D4d);
plot(E4);
plot(F4);
plot(F4d);
plot(G4);
plot(G4d);
plot(A4);
plot(A4d);
plot(B4);
xlabel("Echantillon");
ylabel("Amplitude numérique");
legend("C4","C4#","D4","D4#","E4","F4","F4#","G4","G4#","A4","A4#","B4");
hold off;
