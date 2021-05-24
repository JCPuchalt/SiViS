function [dias, cond, placas] = read_folder_extructure(path, slash)
    files_days=dir(path);
	dias = [];
    for i=1:size(files_days,1)
        if contains(files_days(i).name,'dia_')
            subCadenas = strsplit(files_days(i).name,'_');
            dias = [dias; str2double(subCadenas{2})];
        end
    end
    dias = sort(dias);

    dia_str=strcat('dia_',num2str(dias(1)));
    path_dia = strcat(strcat(path,dia_str),slash);
    ar_cond=dir(path_dia);
    cond = [];
    for i=1:size(ar_cond,1)
        if contains(ar_cond(i).name,'cond_')
            subCadenas = strsplit(ar_cond(i).name,'_');
            cond = [cond; subCadenas{2}];
        end
    end
    cond = sort(cond);

    cond_str = strcat('cond_',cond(1));
    path_dia_cond = strcat(strcat(path_dia,cond_str),slash);
    ar_placas=dir(path_dia_cond);
    placas = [];
    for i=1:size(ar_placas,1)
        if contains(ar_placas(i).name,'placa_')
            subCadenas = strsplit(ar_placas(i).name,'_');
            placas = [placas; str2double(subCadenas{2})];
        end
    end
    placas = sort(placas);

end