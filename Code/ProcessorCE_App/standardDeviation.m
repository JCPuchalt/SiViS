function [mija,SIGMA] = standardDeviation(Medicio)

Referencia = sum(Medicio,2);
poblacio = Referencia(1);
mija = Referencia/poblacio;

numero_dies=size(Medicio,1);
numero_condicions=size(Medicio,2);

for cond=1:numero_condicions
    medicions(:,cond)=Medicio(:,cond)./Medicio(1,cond);
end

grados_libertad = numero_condicions - 1; 

for i=1:numero_dies sumatori(i,:) = (medicions(i,:) - mija(i)).^2; end

SIGMA = sqrt( 1/grados_libertad * sum( sumatori, 2 ));
end