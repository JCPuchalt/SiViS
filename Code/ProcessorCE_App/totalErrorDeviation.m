function [mija,SIGMA,textX] = totalErrorDeviation(Medicio, Referencia)

matriuErrors = Medicio - Referencia;
poblacio = sum(Referencia, 2);
numero_dies=size(matriuErrors,1);
numero_condicions=size(matriuErrors,2);

matriuErrorsPERCENT=[];
for i=1:numero_dies
    matriuErrorsPERCENT(i,:) = matriuErrors(i,:)./Referencia(1,:);
end

errorsTotals = (sum(matriuErrorsPERCENT,2));
mija=(errorsTotals/numero_condicions);
grados_libertad = numero_condicions-1; 

sumatori=[];
for i=1:numero_dies
    sumatori(i,:) = (matriuErrorsPERCENT(i,:) - mija(i)).^2;
end

SIGMA = sqrt( 1/grados_libertad * sum( sumatori, 2 ));

end