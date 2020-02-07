function test_Sd(path,n_variables)

    alpha = ["A","B","C","D","E","F","G","H","I", ...
            "J","K","L","M","N","O","P","Q","R", ...
            "S","T","U","V","W","X","Y","Z"];
    all_data = decode_Sd(path,alpha(1:n_variables));
    counter_error = 0;
    counter_ok = 0;
    for variable = 1:n_variables
        data = all_data.(alpha(variable));
        dif = abs(diff(data));
        moda = abs(mode(dif));
        for i = 1:(size(data,2)-1)
            if ~(dif(1,i)-moda==0)
               counter_error = counter_error + 1;
            else
               counter_ok = counter_ok + 1;
           end
        end
    end
    total_writes = counter_error+counter_ok;
    
    fprintf('Total de números escritos = %.0f k\n', total_writes/1000);
    fprintf('Erros de escrita = %.3f k\n', counter_error/1000);
    fprintf('Acertos de escrita = %.3f k\n', counter_ok/1000);
    fprintf('Confiabilidade = %.2f %%\n', counter_ok/total_writes*100);
    figure(n_variables)
    plot(all_data.A/15000)
    title("Dados gravados - "+n_variables)
end
