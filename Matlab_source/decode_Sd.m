
function result = decode_Sd(path,names) 

    n_variables = size(names,2);
    fileID = fopen(path,'r');
    read = fscanf(fileID,"%c");
    read = dec2hex(unicode2native(read, 'CP1252'));
    fclose("all");
    hexa = reshape(read.',8,[]).';
    for i = 1:size(hexa,1) 
        bytes = [hex2dec(hexa(i,1:2)),hex2dec(hexa(i,3:4)),hex2dec(hexa(i,5:6)),hex2dec(hexa(i,7:8))];   
        bytes = uint8(bytes);
        num(i) = typecast(fliplr(bytes),'single');
    end
%     bytes = reshape(hex2dec({hexa}),[],4);
%     num = typecast(fliplr(bytes),'single');
    counter = 1;
    for i = 1:floor(size(num,2)/n_variables)
        for j = 1:n_variables
            result.(names(j))(i)= num(counter);
            counter = counter+1;
        end
    end
    
end


